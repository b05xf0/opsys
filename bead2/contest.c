
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "contest.h"
#include "db.h"


void start_contest()
{
  printf("\nKezdodik a verseny...\n");
  int pfd_out_1[2];
  int pfd_out_2[2];
  int pfd_in[2];

  if (pipe(pfd_out_1) == -1) { perror("Hiba a pipe nyitaskor!"); exit(1);}  
  if (pipe(pfd_out_2) == -1) { perror("Hiba a pipe nyitaskor!"); exit(1);}  
  if (pipe(pfd_in) == -1) { perror("Hiba a pipe nyitaskor!"); exit(1);}  

  int inspector_1 = fork();
  if (inspector_1 < 0){perror(":(\n"); exit(1);}
  if (inspector_1 > 0)
  {
    int inspector_2 = fork();
    if (inspector_2 < 0){perror(":(\n"); exit(1);}
    if (inspector_2 == 0)
    {
      close(pfd_out_1[1]);
      close(pfd_out_2[0]);
      close(pfd_out_2[1]);
      close(pfd_in[0]);
      inspector(1, pfd_out_1[0], pfd_in[1]);
    }
    else
    {/*
      close(pfd_out_1[0]);
      close(pfd_out_1[1]);
      close(pfd_out_2[0]);
      close(pfd_out_2[1]);
      close(pfd_in[0]);
      close(pfd_in[1]);*/
      Bunny* winner;

      close(pfd_out_1[0]);
      close(pfd_out_2[0]);
      close(pfd_in[1]);
    boss(winner,pfd_out_1[1],pfd_out_2[1],pfd_in[0]);
    }
  }
  else
  {
      close(pfd_out_1[1]);
      close(pfd_out_1[0]);
      close(pfd_out_2[1]);
      close(pfd_in[0]);
    inspector(2, pfd_out_2[0], pfd_in[1]);
  }
}


void boss(Bunny* winner,int pfd_out_1,int pfd_out_2,int pfd_in)
{
  int m_1=1;
  int m_2=2;
  int r;
  printf("\nboss kuld...\n");
  write(pfd_out_1,&m_1,7);
  write(pfd_out_2,&m_2,7);
  close(pfd_out_1);
  close(pfd_out_2);
  while(read(pfd_in,&r,sizeof(r))>0){
    sleep(1);
    printf("visszakap: %i\n", r);
  }
  close(pfd_in);
  printf("\nboss vege...\n");
}

void inspector(int inspector_id, int pfd_in, int pfd_out)
{
  int m;
  printf("\n%i jelen...\n", inspector_id);
  while(read(pfd_in,&m,sizeof(m))>0){
    sleep(1);
    printf("\n%i visszakuld...\n", inspector_id);
    write(pfd_out,&m,sizeof(m));
  }
  close(pfd_in);
  close(pfd_out);
  printf("\n%i vege...\n", inspector_id);
  exit(0);
 
}