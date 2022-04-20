
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include "contest.h"
#include "db.h"

#define LINE_FROM_BOSS(X) pfd_out[(X)][0] 
#define LINE_TO_INSPECTOR(X) pfd_out[(X)][1] 
#define LINE_FROM_INSPECTORS pfd_in[0] 
#define LINE_TO_BOSS pfd_in[1] 
#define N_INSPECTOR 2

static int pfd_out[N_INSPECTOR][2];
static int pfd_in[2];

//int line_from_boss(int inspector) { return pfd_out[inspector][0]; }
//int line_to_inspector(int inspector) { return pfd_out[inspector][1]; }
//int line_from_inspectors() { return pfd_in[0]; }
//int line_to_boss() { return pfd_in[1]; }

void inspectors_job(int);
int send_job(Bunny*);

void run_contest()
{
  pid_t child;
  Bunny* winner;
  Bunny* rec;
  int max_cnt = 0;
  srand(time(NULL));
  printf("\nKezdodik a verseny...\n");
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
      exit(0);
    }
    else
    {
      close(LINE_FROM_BOSS(inspector));
      
      printf("Boss send:%i\n",inspector);
      write(LINE_TO_INSPECTOR(inspector),&inspector,sizeof(inspector));
      close(LINE_TO_INSPECTOR(inspector));
    } 
  /*
  sel(0, &send_job);
  while(read(LINE_FROM_INSPECTORS,rec,sizeof(*rec))>0)
  {
    printf("%s -> %i db tojás\n",rec->name,rec->cnt);
    if (rec->cnt > max_cnt)
    {
      max_cnt = rec->cnt;
      winner = rec;
    }
  }
  printf("Nyertes: %s -> %i db tojás\n",winner->name,winner->cnt);
  */
  }
  int f;
  close(LINE_TO_BOSS);
  while(read(LINE_FROM_INSPECTORS,&f,sizeof(f))>0)
  {
    printf("Boss receive: %i\n",f);
  }
  printf("Boss finished\n");
  close(LINE_FROM_INSPECTORS);
  
  while(wait(NULL)>0);
}

/*
int send_job(Bunny* rec)
{
  printf("%s adatai elküldve: %i\n", rec->name, LINE_TO_INSPECTOR(area_inspector(rec->area)));
  int f = write(LINE_TO_INSPECTOR(area_inspector(rec->area)),rec,sizeof(*rec));
}
*/
void inspectors_job(int inspector)
{
  int f;
  while((read(LINE_FROM_BOSS(inspector),&f,sizeof(f)))>0)
  {
    printf("Inspector %i receive:%i\n",inspector,f);
    write(LINE_TO_BOSS,&f,sizeof(f));
    printf("Inspector %i send:%i\n",inspector,f);
  }
  printf("Inspector %i finished\n",inspector);




  /*Bunny* rec;
  printf("Felugyelo(%i) jelen...%i\n", inspector, LINE_FROM_BOSS(inspector));
  int f;
  while((f=read(LINE_FROM_BOSS(inspector),rec,sizeof(*rec)))>0)
  {
    printf("%s adatai fogadva\n", rec->name);
    rec->cnt = rand() % 100 + 1;
    write(LINE_TO_BOSS,rec,sizeof(*rec));
  }
  printf("méret: %i\n", f);

  printf("Felugyelo(%i) végzett...\n", inspector);
  */
}
