
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include "contest.h"
#include "db.h"

#define LINE_FROM_BOSS(X) pfd_out[(X)][0] 
#define LINE_TO_INSPECTOR(X) pfd_out[(X)][1] 
#define LINE_FROM_INSPECTORS pfd_in[0] 
#define LINE_TO_BOSS pfd_in[1] 
#define N_INSPECTOR 2

static int pfd_out[N_INSPECTOR][2];
static int pfd_in[2];

void inspectors_job(int);
int send_job(Bunny*);

void run_contest()
{
  pid_t child;
  Bunny winner;
  Bunny rec;
  int max_cnt = 0;
  printf("\nFonyuszi: kezdodik a verseny...\n\n");
  if (pipe(pfd_in) == -1) { perror(":(\n"); exit(1);}
  for (int inspector = 0; inspector < N_INSPECTOR; ++inspector)
  {
    if (pipe(pfd_out[inspector]) == -1) { perror(":(\n"); exit(1);} 
    if ((child = fork()) < 0) {perror(":(\n"); exit(1);}
    if (child == 0)
    {
      close(LINE_FROM_INSPECTORS);
      close(LINE_TO_INSPECTOR(inspector));
      inspectors_job(inspector);
      close(LINE_FROM_BOSS(inspector));
      close(LINE_TO_BOSS);
      _exit(0);
    }
    else
    {
      close(LINE_FROM_BOSS(inspector));
    } 
  }
  close(LINE_TO_BOSS);
  sel(0, &send_job);
  
  for (int inspector=0; inspector < N_INSPECTOR; ++inspector)
  {
    close(LINE_TO_INSPECTOR(inspector));
  }
  
  while(read(LINE_FROM_INSPECTORS,&rec,sizeof(rec))>0)
  {
    printf("Fonyuszi: %s eredmenye (%i) fogadva\n",rec.name,rec.cnt);
    if (rec.cnt > max_cnt)
    {
      max_cnt = rec.cnt;
      winner = rec;
    }
  }
  close(LINE_FROM_INSPECTORS);
  while(wait(NULL)>0);
  printf("Fonyuszi: ...veget ert a verseny, %s nyert %i tojással\n", winner.name, winner.cnt);
}

int send_job(Bunny* rec)
{
  write(LINE_TO_INSPECTOR(area_inspector(rec->area)),rec,sizeof(*rec));
  printf("Fonyuszi: %s (%s) adatai elküldve Felugyelo(%i) reszere\n", rec->name, area_name(rec->area), area_inspector(rec->area));
}

void inspectors_job(int inspector)
{
  Bunny rec;
  printf("Felugyelo(%i): munka elkezdese\n", inspector);
  srand(getpid() * time(NULL));
  while((read(LINE_FROM_BOSS(inspector),&rec,sizeof(rec)))>0)
  {
    printf("Felugyelo(%i): %s (%s) adatai fogadva\n", inspector, rec.name, area_name(rec.area));
    rec.cnt = rand() % 100 + 1;
    write(LINE_TO_BOSS,&rec,sizeof(rec));
    printf("Felugyelo(%i): %s (%s) eredmenye (%i) visszakuldve\n", inspector, rec.name, area_name(rec.area), rec.cnt);
  }
  printf("Felugyelo(%i): munka elvegezve\n", inspector);
}
