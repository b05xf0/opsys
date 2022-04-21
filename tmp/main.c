#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

int main(int argc, char* argv[])
{
    int arr[] = {1, 2, 4, 5, 2, 7};
    int arrSize = sizeof(arr) / sizeof(arr[0]);

    int fd[2];
    if(pipe(fd) == -1){
        perror("An error occured while creating the pipe\n");
        return 1;
    }
    
    int fd2[2];
    if(pipe(fd2) == -1){
    perror("An error occured while creating the pipe\n");
        return 1;
    }

    int child1 = fork();
    if(child1 == -1){
        perror("An error occured while creating child process\n");
        return 2;
    }
    
    if(child1 == 0){
        printf("First child process starts\n");
        close(fd[0]);
        int sum = 0;
        int i;
        for(i = 0; i < arrSize / 2; ++i){
            sum += arr[i];
        }
        printf("The first child process calculated partial sum: %d\n", sum);
        if(write(fd[1], &sum, sizeof(sum)) == -1){
            perror("An error occured while writing into the pipe\n");
            return 3;
        }
        close(fd[1]);
        printf("First child has written %d into the pipe and closed it\n", sum);
        printf("First child process ended\n");
    }
    else{
        int child2 = fork();
        if(child2 == -1){
            perror("An error occured while creating child process\n");
            return 2;
        }
        
        if(child2 == 0){
            close(fd2[0]);
            int sum = 0;
            int i;
            for(i = arrSize / 2; i < arrSize; ++i){
                sum += arr[i];
            }
            printf("The second child process calculated partial sum: %d\n", sum);
            if(write(fd2[1], &sum, sizeof(sum)) == -1){
                perror("An error occured while writing into the pipe\n");
                return 3;
            }
            close(fd2[1]);
            printf("Second child process ended\n");
        }
        else{
            close(fd[1]);
            int sumFromFirstChild;
            while(read(fd[0], &sumFromFirstChild, sizeof(sumFromFirstChild)) > 0){
                printf("Partial sum arrived from first child: %d\n", sumFromFirstChild);
            }
            printf("Partial sum arrived from first child: %d\n", sumFromFirstChild);
            close(fd[0]);
        
            close(fd2[1]);
            int sumFromSecondChild;            
            if(read(fd2[0], &sumFromSecondChild, sizeof(sumFromSecondChild)) == -1){
                perror("An error occured while reading from the pipe\n");
                return 4;
            }
            printf("Partial sum arrived from second child: %d\n", sumFromSecondChild);
            close(fd2[0]);
        }
        wait(NULL);
    }

    return 0;

}