#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>

void handler(int signumber){
    //printf("A kezelo fogadta a kovetkezo jelet: %d\n", signumber);
}

//struct sigaction sigact;

typedef struct{
    long mtype;
    char mtext[1024];
} Message;

int main(int argc, char* argv[])
{
    int status;

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    // uzenetsor beallitas
    int messagequeue;
    key_t key;

    key = ftok(argv[0], 1);
    messagequeue = msgget(key, 0600 | IPC_CREAT);
    if(messagequeue < 0){
        perror("Nem sikerult uzenetsort kesziteni\n");
        exit(EXIT_FAILURE);
    }

    // csovek
    int mtb[2];
    int ttm[2];
    int btm[2];
    int fishpipe[2];

    if(pipe(mtb) == -1 || pipe(ttm) == -1 || pipe(btm) == -1 || pipe(fishpipe) == -1){
        perror("Nem sikerult a csovek letrehozasa\n");
        exit(EXIT_FAILURE);
    }

    // szemafor
    sem_t* semid;
    char* sem_name = "/zelln3";

    semid = sem_open(sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(semid == SEM_FAILED){
        perror("Nem sikerult szemafort kesziteni\n");
        exit(EXIT_FAILURE);
    }

    pid_t tutajos;
    tutajos = fork();
    if(tutajos == -1){
        perror("Tutajos megbetegedett\n");
        exit(EXIT_FAILURE);
    }

    if(tutajos > 0){
        // Matula
        pid_t butyok;
        butyok = fork();
        if(butyok == -1){
            perror("Butyok megbetegedett\n");
            exit(EXIT_FAILURE);
        }
        if(butyok > 0){
            // Matula
            printf("Matula elkuldi a gyerekeket a feladatra\n");
            
            sigset_t sigset;
            sigfillset(&sigset);
            sigdelset(&sigset, SIGUSR1);
            sigdelset(&sigset, SIGUSR2);
            sigsuspend(&sigset);

            printf("Matula veszi Tutajos jelzeset\n");
            printf("Matula veszi Butyok jelzeset\n");

            // uzenet kuldes
            Message msg1 = {1, "Fogj 3 halat!"};
            Message msg2 = {2, "Gyujts 2 koteg fat!"};

            if(msgsnd(messagequeue, &msg1, sizeof(msg1.mtext), 0) < 0){
                perror("Matula nem tud uzenni Tutajosnak\n");
                exit(EXIT_FAILURE);
            }

            if(msgsnd(messagequeue, &msg2, sizeof(msg2.mtext), 0) < 0){
                perror("Matula nem tud uzenni Butyoknek\n");
                exit(EXIT_FAILURE);
            }

            printf("Matula befejezte az elokeszuleteket\n");

            // Matula uzenetet fogad es uzen
            close(mtb[0]);
            close(btm[1]);
            close(ttm[1]);
            char msgButyok[100];
            char msgTutajos[100];
            
            if(read(btm[0], &msgButyok, sizeof(msgButyok)) == -1){
                perror("Matula nem tudja fogadni Butyok uzenetet\n");
                exit(EXIT_FAILURE);
            }
            printf("Matula fogadta Butyok uzenetet: %s\n", msgButyok);
            
            if(read(ttm[0], &msgTutajos, sizeof(msgTutajos)) == -1){
                perror("Matula nem tudja fogadni Tutajos uzenetet\n");
                exit(EXIT_FAILURE);
            }
            printf("Matula fogadta Tutajos uzenetet: %s\n", msgTutajos);

            char answerButyok[100] = "Hagyja Bela a vizes fat masnak, segitsen a horgasznak!\n";
            if(write(mtb[1], &answerButyok, sizeof(answerButyok)) == -1){
                perror("Matula nem tud uzenetet kuldeni\n");
                exit(EXIT_FAILURE);
            }

            close(mtb[1]);
            close(btm[0]);
            close(ttm[0]);
            
            close(fishpipe[1]);
            int dinner;
            if(read(fishpipe[0], &dinner, sizeof(int)) == -1){
                perror("Nem erkeztek meg a halak\n");
                exit(EXIT_FAILURE);
            }
            close(fishpipe[0]);
            
            waitpid(tutajos, &status, 0);
            waitpid(butyok, &status, 0);

            printf("Matula megkapta a %d halat\n", dinner);
            int f = open("fish.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
            if(f == -1){
                perror("Nem sikerult megnyitni a fajlt\n");
                exit(EXIT_FAILURE);
            }
            sem_wait(semid);
            if(write(f, &dinner, sizeof(int)) == -1){
                perror("Nem sikerult a fajlba inri\n");
                exit(EXIT_FAILURE);
            }
            sem_post(semid);
            sem_unlink(sem_name);
            printf("Matula felirta a halak szamat\n");

        }
        else{
            // Butyok
            sleep(1);

            printf("Butyok jelez Matulanak\n");
            kill(getppid(), SIGUSR2);

            // uzenet olvasas
            Message msg;
            if(msgrcv(messagequeue, &msg, sizeof(msg.mtext), 2, 0) == -1){
                perror("Butyok nem tudta fogadni az uzenetet\n");
                exit(EXIT_FAILURE);
            }
            printf("Butyok uzenete: %s\n", msg.mtext);
            printf("Butyok befejezte az elokeszuleteket\n");

            // Butyok uzen es uzenetet fogad
            close(btm[0]);
            close(mtb[1]);
            char msgButyokMatula[100] = "Vizes a fa!";
            if(write(btm[1], &msgButyokMatula, sizeof(msgButyokMatula)) == -1){
                perror("Butyok nem tud uzenetet kuldeni\n");
                exit(EXIT_FAILURE);
            }

            char answerMatula[100];
            if(read(mtb[0], &answerMatula, sizeof(answerMatula)) == -1){
                perror("Butyok nem tudja fogadni Matula uzenetet\n");
                exit(EXIT_FAILURE);
            }
            printf("Butyok fogadta Matula uzenetet: %s", answerMatula);

            close(btm[1]);
            close(mtb[0]);

            close(fishpipe[0]);
            srand(getpid() * time(NULL));
            int fish = rand() % 2 + 2;

            if(write(fishpipe[1], &fish, sizeof(int)) == -1){
                perror("Butyok elvesztette a halakat\n");
                exit(EXIT_FAILURE);
            }
            close(fishpipe[1]);
        }
    }
    else{
        // Tutajos
        sleep(1);
        printf("Tutajos jelez Matulanak\n");

        kill(getppid(), SIGUSR1);

        // uzenet olvasas
        Message msg;
        if(msgrcv(messagequeue, &msg, sizeof(msg.mtext), 1, 0) == -1){
            perror("Tutajos nem tudta fogadni az uzenetet\n");
            exit(EXIT_FAILURE);
        }
        printf("Tutajos uzenete: %s\n", msg.mtext);
        printf("Tutajos befejezte az elokeszuleteket\n");

        // Tutajos halat fog
        srand(getpid() * time(NULL));
        int fish = rand() % 2 + 1;

        // Tutajos uzen es uzenetet fogad
        close(ttm[0]);
        char msgTutajosMatula[100] = "A csuka megfogott stop, segitseg stop!";
        if(write(ttm[1], &msgTutajosMatula, sizeof(msgTutajosMatula)) == -1){
            perror("Tutajos nem tud uzenetet kuldeni\n");
            exit(EXIT_FAILURE);
        }
        close(ttm[1]);
    }

    return 0;
}