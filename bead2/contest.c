#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>
#include "contest.h"
#include "db.h"

#define LINE_FROM_BOSS(X) pfd_out[(X)][0] 
#define LINE_TO_INSPECTOR(X) pfd_out[(X)][1] 
#define LINE_FROM_INSPECTORS pfd_in[0] 
#define LINE_TO_BOSS pfd_in[1] 

static int pfd_out[N_INSPECTOR][2];
static int pfd_in[2];

void inspectors_job(int);
int send_job(Bunny*);

void run_contest(Bunny* winner)
{
  Bunny rec;
  int max_cnt = 0;
  printf("\n"LOG_BOSS"kezdodik a verseny\n");
  if (pipe(pfd_in) == -1){
    perror("Hiba: a kommunikacios csatorna megnyitasa nem sikerult\n");
    exit(EXIT_FAILURE);
  }
  for (int inspector = 0; inspector < N_INSPECTOR; ++inspector)
  {
    if (pipe(pfd_out[inspector]) == -1)
    {
      perror("Hiba: a kommunikacios csatorna megnyitasa nem sikerult\n");
      exit(EXIT_FAILURE);
    }
    switch (fork())
    {
      case -1:
        perror("Hiba: egy felugyelo megbetegedett\n");
        exit(EXIT_FAILURE);
      case 0:
        close(LINE_FROM_INSPECTORS);
        close(LINE_TO_INSPECTOR(inspector));
        inspectors_job(inspector);
        close(LINE_FROM_BOSS(inspector));
        close(LINE_TO_BOSS);
        _exit(EXIT_SUCCESS);
      default:
        close(LINE_FROM_BOSS(inspector));   
    }
  }
  close(LINE_TO_BOSS);
  printf(LOG_BOSS"osszes resztvevo (%i) adatai elkuldve\n",
         sel(0, &send_job)); 
  for (int inspector=0;
       inspector < N_INSPECTOR;
       close(LINE_TO_INSPECTOR(inspector++)));
  while(read(LINE_FROM_INSPECTORS, &rec, sizeof rec) > 0)
  {
    printf(LOG_BOSS"%s eredmenye (%i) fogadva\n", rec.name, rec.cnt);
    if (rec.cnt > max_cnt)
    {
      max_cnt = rec.cnt;
      *winner = rec;
    }
  }
  close(LINE_FROM_INSPECTORS);
  while(wait(NULL) > 0);
  printf(LOG_BOSS"veget ert a verseny\n");
}

int send_job(Bunny* rec)
{
  write(LINE_TO_INSPECTOR(area_inspector(rec->area)), rec, sizeof *rec);
  printf(LOG_BOSS"%s (%s) adatai elkuldve Felugyelo(%i) reszere\n",
         rec->name, area_name(rec->area), area_inspector(rec->area));
  return 1;
}

void inspectors_job(int inspector)
{
  Bunny rec;
  printf(LOG_INSPECTOR"munka elkezdve\n", inspector);
  srand(getpid() * time(NULL));
  while((read(LINE_FROM_BOSS(inspector), &rec, sizeof rec)) > 0)
  {
    printf(LOG_INSPECTOR"%s (%s) adatai fogadva\n",
           inspector, rec.name, area_name(rec.area));
    rec.cnt = rand() % 100 + 1;
    write(LINE_TO_BOSS, &rec, sizeof rec);
    printf(LOG_INSPECTOR"%s (%s) eredmenye (%i) visszakuldve\n",
           inspector, rec.name, area_name(rec.area), rec.cnt);
  }
  printf(LOG_INSPECTOR"munka elvegezve\n", inspector);
}
