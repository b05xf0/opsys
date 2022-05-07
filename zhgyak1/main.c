#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <semaphore.h>
#include <fcntl.h>

#define N_ASSISTANT 2
#define FELDERITES SIGUSR1
#define VAHUR_CSAPAT SIGUSR1
#define FICKO_CSAPAT SIGUSR2

typedef struct { 
     long mtype;
     int mtext; 
} Msg; 

void assistants_job(int);

void sig_from_vuk_handler(int);

void sig_to_vuk_handler(int, siginfo_t*, void*);

static const char* assistants[] = {"Vahur", "Ficko"};
static int pfd[N_ASSISTANT][2];
static int mq;
static sem_t* semid;

int main()
{
  Msg msg;
  int status;
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGUSR1);
  sigaddset(&sigset, SIGUSR2);
  sigprocmask(SIG_BLOCK, &sigset,NULL);
  mq = msgget(ftok("/tmp/b05xf0", 1), 0600 | IPC_CREAT ); 
  if (mq < 0){ perror("msgget"); exit(EXIT_FAILURE); } 
  semid = sem_open("/b05xf0", O_CREAT, S_IRUSR | S_IWUSR, 1);
	if (semid == SEM_FAILED){ perror("sem_open"); exit(EXIT_FAILURE); } 
  printf("Vuk: rendeles elkezdve\n");
  for (int i = 0; i < N_ASSISTANT; ++i)
  {
    int assistant_pid;
    if (pipe(pfd[i]) == -1){ perror("pipe"); exit(EXIT_FAILURE);
    }
    switch(assistant_pid = fork())
    {
      case -1:
        perror("fork");
        exit(EXIT_FAILURE);
      case 0:
        printf("%s: munka elkezdve\n", assistants[i]);
        close(pfd[i][1]);
        assistants_job(i);
        close(pfd[i][0]);
        printf("%s: munka befejezve\n", assistants[i]);
        _exit(EXIT_SUCCESS);
      default:
        close(pfd[i][0]); 
        kill(assistant_pid, FELDERITES);
        printf( "Vuk: FELDERITES jelzes %snak elkuldve \n", assistants[i]);
    }
  }
  for(int i = 0; i < N_ASSISTANT; ++i)
  {
    int status = msgrcv(mq, &msg, 4, (long)(i+1), 0 );
    if (status < 0) perror("msgsnd"); 
    else printf("Vuk: %s uzenete (%i) fogadva\n", assistants[i], msg.mtext);
    int goal = (int)(msg.mtext * .8);
    write(pfd[i][1], &goal, sizeof goal);
    close(pfd[i][1]); 
  }
  struct sigaction sigact;
  sigact.sa_sigaction = sig_to_vuk_handler;
  sigemptyset(&sigact.sa_mask);
  sigaddset(&sigact.sa_mask, VAHUR_CSAPAT);
  sigaddset(&sigact.sa_mask, FICKO_CSAPAT);
  sigact.sa_flags = SA_SIGINFO;
  sigaction(VAHUR_CSAPAT,&sigact,NULL);
  sigaction(FICKO_CSAPAT,&sigact,NULL);
  sigfillset(&sigset);
  sigdelset(&sigset, VAHUR_CSAPAT);
  sigdelset(&sigset, FICKO_CSAPAT);
  sigdelset(&sigset, SIGINT);
  sigsuspend(&sigset);
  sigsuspend(&sigset);
  while(wait(NULL) > 0);
  printf("Vuk: rendeles befejezve\n");
  status = msgctl(mq, IPC_RMID, NULL); 
  if (status < 0) perror("msgctl");
  sem_unlink("/b05xf0");
  return 0;
}

void assistants_job(int i)
{
  sigset_t sigset;
  srand(time(NULL) * getpid());
  sigfillset(&sigset);
  sigdelset(&sigset, FELDERITES);
  sigdelset(&sigset, SIGINT);
  signal(FELDERITES, sig_from_vuk_handler);
  sigsuspend(&sigset);
  printf("%s: FELDERITES jelzes fogadva\n", assistants[i]);
  const Msg msg = {(i+1), rand()%(30-20+1)+20};
  int status = msgsnd(mq, &msg, sizeof msg.mtext, 0 ); 
  if (status < 0) perror("msgsnd");
  else printf("%s: uzenet (%i) elkuldve\n", assistants[i], msg.mtext);
  int goal;
  if (read(pfd[i][0], &goal, sizeof goal) < 0) perror("read");
  else printf("%s: Vuk uzenete (%i) fogadva\n", assistants[i], goal);
  union sigval result = {(int)(goal * .9)};
  sigqueue(getppid(), (i == 0) ? VAHUR_CSAPAT : FICKO_CSAPAT, result);
  printf("%s: jelzes (%i) elkuldve\n", assistants[i], result.sival_int);
}

void sig_from_vuk_handler(int signumber)
{
  //psignal(signumber, "FELDERITES signal handler");
}

void sig_to_vuk_handler(int signumber, siginfo_t* info, void* nonused)
{
  printf("Vuk: %s_CSAPAT jelzes fogadva, eredmeny: (%i)\n",
    (signumber == VAHUR_CSAPAT) ? "VAHUR" : "FICKO",
    info->si_value.sival_int);
  sem_wait(semid);
  int f = open("results.bin", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
  if(f < 0){ perror("open"); exit(EXIT_FAILURE);}
  write(f, &info->si_value.sival_int, sizeof info->si_value.sival_int);
  close(f);
  sem_post(semid);
}