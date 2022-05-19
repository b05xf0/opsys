#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h> 
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/msg.h> 
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/ipc.h>

#define PERM  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH 
#define Nyuszi_start SIGRTMIN
#define Keress_meg SIGUSR1
#define Hazaterhet SIGUSR2
#define PIPE "/tmp/b05xf0p"
#define SEM "/b05xf0sem"

typedef struct { 
  long mtype;
  int mtext; 
} Msg;

typedef struct {
  int area_id;
  int pop;
} Stat;

typedef struct { 
  long mtype;
  Stat mstruct; 
} Resp;

void wait_for_sig(int signumber)
{
  sigset_t sigset;
  sigfillset(&sigset);
  sigdelset(&sigset, signumber);
  sigdelset(&sigset, SIGINT);
  sigsuspend(&sigset);
}

void sig_handler(int signumber)
{
  psignal(signumber, "signal handler");
}

int main(int argc, char* argv[])
{
  //if(argc < 2) exit(EXIT_FAILURE);
  
  const char* areas[] = {
    "Baratfa",
    "Lovas",
    "Kigyos-patak",
    "Szula",
    "Malom telek",
    "Paskom",
    "Kaposztas kert"
  };

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, Nyuszi_start);
  sigaddset(&sigset, Keress_meg);
  sigaddset(&sigset, Hazaterhet);
  sigprocmask(SIG_BLOCK, &sigset, NULL);
  struct sigaction sigact;
  sigact.sa_flags=0;
  
  sigact.sa_handler = sig_handler;

  sigemptyset(&sigact.sa_mask);
  sigaddset(&sigact.sa_mask, Nyuszi_start);
  sigaddset(&sigact.sa_mask, Keress_meg);
  sigaddset(&sigact.sa_mask, Hazaterhet);
  sigaction(Nyuszi_start, &sigact,NULL);

  sigaction(Keress_meg, &sigact,NULL);
  sigaction(Hazaterhet, &sigact,NULL);


  int mq = msgget(IPC_PRIVATE, PERM | IPC_CREAT ); 
  if (mq < 0) perror("msgget");
  
  //sem_t* sem = sem_open(SEM, O_CREAT, PERM, 0);
	//if (sem == SEM_FAILED) perror("sem_open"); 

  //int shm_id = shmget(IPC_PRIVATE, sizeof(int), PERM | IPC_CREAT);

  pid_t child1_pid = fork();
  if(child1_pid < 0) perror("fork");
  if(child1_pid > 0)
  {
    pid_t child2_pid = fork();
    if(child2_pid < 0) perror("fork"); 
    if(child2_pid > 0)
    { // Fonyuszi
      int stats_old[7];
      int stats_new[7];
      wait_for_sig(Nyuszi_start);
      wait_for_sig(Nyuszi_start);
      srand(time(NULL) * getpid());
      for(int i = 0; i < sizeof(areas) / sizeof(areas[0]); ++i)
      {
        Msg msg = {(i < 3) ? 1 : 2, i};
        stats_old[i] = rand()%51 + 50;
        msgsnd(mq, &msg, sizeof msg.mtext, 0);
      }
      for(int i = 0; i < sizeof(areas) / sizeof(areas[0]); ++i)
      {
        Resp resp;
        msgrcv(mq, &resp, sizeof resp.mstruct, 3, 0);
        printf("Fonyuszi: %s lakossaga %i (%s)\n", areas[resp.mstruct.area_id], resp.mstruct.pop, (stats_old[resp.mstruct.area_id] > resp.mstruct.pop ) ? "fogyas" : "gyarapodas/valtozatlan");
      }
      int status;
      waitpid(child1_pid,&status,0);
      printf("Fonyuszi: Tapsi nyugovora tert\n");
      waitpid(child2_pid,&status,0);
      printf("Fonyuszi: Fules nyugovora tert\n");
      printf("Fonyuszi: en is vegeztem\n");
      //exit(EXIT_SUCCESS);
    }
    else
    { // Fules
      srand(time(NULL) * getpid());
      kill(getppid(), Nyuszi_start);
      for(int i = 0; i < 4; ++i)
      {
        Msg msg;
        msgrcv(mq, &msg, sizeof msg.mtext, 2, 0);
        printf("Fules: %s uzenet fogadva\n", areas[msg.mtext]);
        Stat stat = {msg.mtext, rand()%51 + 50};
        Resp resp = {3, stat};
        msgsnd(mq, &resp, sizeof resp.mstruct, 0);
      }
      _exit(EXIT_SUCCESS);
    }
  }
  else
  { // Tapsi
    srand(time(NULL) * getpid());
    kill(getppid(), Nyuszi_start); 
      for(int i = 0; i < 3; ++i)
      {
        Msg msg;
        msgrcv(mq, &msg, sizeof msg.mtext, 1, 0);
        printf("Tapsi: %s uzenet fogadva\n", areas[msg.mtext]);
        Stat stat = {msg.mtext, rand()%51 + 50};
        Resp resp = {3, stat};
        msgsnd(mq, &resp, sizeof resp.mstruct, 0);
      }
      
    _exit(EXIT_SUCCESS);
  }
  msgctl(mq, IPC_RMID, NULL);
  //sem_unlink(SEM);
  return 0;
}