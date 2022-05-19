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
#define L_AREA 1024
#define LINE_TO_TAPSI "/tmp/b05xf0p_1"
#define LINE_TO_FULES "/tmp/b05xf0p_2"
#define LINE_TO_FONYUSZI "/tmp/b05xf0p_3"


#define SEM "/b05xf0sem"

typedef struct { int area; int pop; } Stat;

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
  signal(HAZATERHET, sig_handler);

  if(mkfifo(LINE_TO_TAPSI, PERM) == -1) perror("mkfifo");
  if(mkfifo(LINE_TO_FULES, PERM) == -1) perror("mkfifo");
  if(mkfifo(LINE_TO_FONYUSZI, PERM) == -1) perror("mkfifo");

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
        int fdw = open(LINE_TO_FONYUSZI, O_WRONLY);
        int fdr = open((i==0) ? LINE_TO_TAPSI : LINE_TO_FULES, O_RDONLY);
        srand(time(NULL) * getpid());
        kill(getppid(), NYUSZI_START);
        
        char task[L_AREA];
        if(read(fdr, task, L_AREA) < 0) perror("read");
        close(fdr);
        printf("%s: az én területem: %s\n", name, task);
        const Stat response = {i, rand() % 61 + 40};
        write(fdw, &response, sizeof response);
        close(fdw);
        wait_for_response();
        int* shresults = shmat(shm_id,NULL,0);
        shresults[i] = rand() % 31 + 70;
        sem_post(sem);
        _exit(EXIT_SUCCESS);
      default: break;
    }
  }
  strcpy(name,"Főnyuszi");
  int fdr = open(LINE_TO_FONYUSZI, O_RDONLY);
  int fdws[2];
  for(int i = 0; i < 2; ++i)
  {
    fdws[i] = open((i==0) ? LINE_TO_TAPSI : LINE_TO_FULES, O_WRONLY);
    wait_for_start();
  }
  int results[] = {-1, -1};
  const int old_results[] = {rand() % 51 + 50, rand() % 51 + 50};
  printf("%s: mindenki készen áll\n", name);
  const char tasks[][L_AREA] = {
    "{Barátfa, Lovas, Kígyós patak}",
    "{Káposztás, Szula, Malom telek, Páskom}"
  };
  for(int i = 0; i < 2; ++i)
  {
    write(fdws[i], tasks[i], L_AREA);
    close(fdws[i]);
  }
  int* shresults = shmat(shm_id,NULL,0);
  Stat response;
  while(read(fdr, &response, sizeof response) > 0)
  {
    int j = response.area;
    int pop = response.pop;    
    printf("%s: %s terület lakossága: %i (%i)\n", name,
      tasks[j], pop, pop - old_results[j]);
    if(pop - old_results[j] < 0)
    {
      kill(child_pids[j], KERESS_MEG);
      sem_wait(sem);
      pop = shresults[j];
      printf("%s: 2. számlálás után %s terület lakossága: %i (%i)\n", name,
      tasks[j], pop, pop - old_results[j]);
    }
    else kill(child_pids[j], HAZATERHET);
    results[j] = pop;
  }
  close(fdr);
  int total = 0, change = 0;
  for(int i = 0; i < 2; ++i)
  {
    total += results[i];
    change += results[i] - old_results[i];
  }
  printf("%s: a nyusziszámlálás végeredménye: %i (%i)\n", name,
    total, change);
  shmdt(shresults);
  while(wait(NULL) > 0);
  sem_unlink(SEM);
  unlink(LINE_TO_FONYUSZI);
  unlink(LINE_TO_TAPSI);
  unlink(LINE_TO_FULES);
  printf("%s: mindenki végzett, térjünk nyugovóra\n", name);
  return 0;
}

void sig_handler(int signumber)
{
  if(signumber == HAZATERHET) _exit(EXIT_SUCCESS);
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
