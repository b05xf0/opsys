#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define NYUSZI_START SIGUSR1
#define KERESS_MEG SIGUSR2
#define HAZATERHET SIGTERM
#define PERM  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH 
#define L_NAME 20
#define SEM "/b05xf0sem"

typedef struct { long mtype; char mtext[1024]; } Task;
typedef struct { int area; int pop; } Stat;
typedef struct { long mtype; Stat mstruct; } Response;

void sig_handler(int);
void wait_for_start(void);
void wait_for_response(void);

int main(int argc, char* argv)
{
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, NYUSZI_START);
  sigaddset(&sigset, KERESS_MEG);
  sigaddset(&sigset, HAZATERHET);
  sigprocmask(SIG_BLOCK, &sigset, NULL);
  signal(NYUSZI_START, sig_handler);
  signal(KERESS_MEG, sig_handler);

  int mq = msgget(IPC_PRIVATE, PERM | IPC_CREAT);

  sem_t* sem = sem_open(SEM, O_CREAT, PERM, 0);
	if (sem == SEM_FAILED) perror("sem_open"); 

  int shm_id = shmget(IPC_PRIVATE, sizeof(int) * 2, PERM | IPC_CREAT);

  char name[L_NAME];
  pid_t child_pids[2];
  for(int i = 0; i < 2; ++i)
  {
    int sig;
    switch(child_pids[i] = fork())
    {
      case -1: perror("fork"); break;
      case 0:
        strcpy(name, (i == 0) ? "Tapsi" : "Füles");        
        srand(time(NULL) * getpid());
        // ----- 1. feladat ------
        kill(getppid(), NYUSZI_START);
        Task task;
        msgrcv(mq, &task, sizeof task.mtext, (long)(i + 1), 0);
        printf("%s: az én területem: %s\n", name, task.mtext);
        // ----- 2. feladat ------
        const Response response = {3, {i, rand() % 51 + 50}};
        msgsnd(mq, &response, sizeof response.mstruct, 0);
        // ----- 3. feladat ------
        wait_for_response();
        const Response second_response = {3, {i, rand() % 31 + 70}};
        msgsnd(mq, &second_response, sizeof second_response.mstruct, 0);
        // ----- vege ------------
        _exit(EXIT_SUCCESS);
      default: wait_for_start();
    }
  }

  strcpy(name,"Főnyuszi");
  // ----- 1. feladat ------
  printf("%s: mindenki készen áll\n", name);
  const Task tasks[] = {
    {1, "{Barátfa, Lovas, Kígyós patak}"},
    {2, "{Káposztás, Szula, Malom telek, Páskom}"}
  };
  // ----- 2. feladat ------
  const int old_results[] = {rand() % 51 + 50, rand() % 51 + 50};
  int* results = shmat(shm_id,NULL,0);
  for(int i = 0; i < 2; ++i)
  {
    results[i] = -1;
    msgsnd(mq, &tasks[i], strlen(tasks[i].mtext) + 1, 0);
  }
  for(int i = 0; i < 2; ++i)
  {
    Response response;
    msgrcv(mq, &response, sizeof response.mstruct, 3, 0);
      int j = response.mstruct.area;
      int pop = response.mstruct.pop;
    printf("%s: %s terület lakossága: %i (%i)\n", name,
      tasks[j].mtext, pop, pop - old_results[j]);
  // ----- 3. feladat ------  
    if(results[j] < 0 && pop - old_results[j] < 0)
    {
      kill(child_pids[j], KERESS_MEG);
      --i;
    }
    else kill(child_pids[j], HAZATERHET);
    results[j] = pop;
  }
  // ----- 4. feladat ------
  sem_post(sem);
  int total = 0, change = 0;
  for(int i = 0; i < 2; ++i)
  {
    total += results[i];
    change += results[i] - old_results[i];
  }
  printf("%s: a nyusziszámlálás végeredménye: %i (%i)\n", name,
    total, change);
  shmdt(results);
  // ----- vege ------------
  while(wait(NULL) > 0);
  msgctl(mq, IPC_RMID, NULL);
  sem_unlink(SEM);
  printf("%s: mindenki végzett, térjünk nyugovóra\n", name);
  return 0;
}

void sig_handler(int signumber)
{
  //psignal(signumber, "signal handler");
}

void wait_for_start()
{
  sigset_t sigset;
  sigfillset(&sigset);
  sigdelset(&sigset, NYUSZI_START);
  sigdelset(&sigset, SIGINT);
  sigsuspend(&sigset);
}

void wait_for_response()
{
  sigset_t sigset;
  sigfillset(&sigset);
  sigdelset(&sigset, KERESS_MEG);
  sigdelset(&sigset, HAZATERHET);
  sigdelset(&sigset, SIGINT);
  sigsuspend(&sigset);
}
