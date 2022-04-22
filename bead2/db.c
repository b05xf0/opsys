#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "db.h"

#define F_BUNNIES "bunnies.dat"

static const Area areas[] = {
  {"Baratfa", 0},
  {"Lovas", 0},
  {"Szula", 1},
  {"Kigyos-patak", 0},
  {"Malom telek", 1},
  {"Paskom", 1},
  {"Kaposztas kert", 0}
};

int open_db(int access_mode)
{
  int f = open(F_BUNNIES, access_mode | O_CREAT, S_IRUSR | S_IWUSR);
  if(f < 0)
  {
    perror("Hiba: nem sikerult megnyitni az adatbazist\n");
    exit(EXIT_FAILURE);
  }
  return f;
}

void write_db(int f, Bunny* rec)
{
  if(write(f, rec, sizeof *rec) != sizeof *rec)
  {
    perror("Hiba: az adatok mentese nem sikerult\n");
    exit(EXIT_FAILURE);      
  }  
}

void ins(Bunny* new_rec)
{
  int f = open_db(O_RDWR);
  Bunny rec;
  rec.is_deleted = false;
  while (read(f, &rec, sizeof rec) && !rec.is_deleted);
  if (rec.is_deleted)
    lseek(f, -(sizeof rec), SEEK_CUR);
  write_db(f, new_rec);
  close(f);
}

void upd(int idx, Bunny* rec)
{
  int f = open_db(O_WRONLY);
  lseek(f, idx * sizeof *rec, SEEK_SET);
  write_db(f, rec);
  close(f);
}

int sel(int area, int (*fun)(Bunny*))
{
  int f = open_db(O_RDONLY);
  Bunny rec;
  int c = 0;
  while (read(f, &rec, sizeof rec))
    if (!rec.is_deleted && (0 == area || rec.area == area))
      c += fun(&rec);
  close(f);
  return c;
}

int src(Bunny* s_rec)
{
  int f = open_db(O_RDONLY);
  Bunny rec;
  int idx = 0;
  bool found = false;
  while (read(f, &rec, sizeof rec) &&
         !(found = (!rec.is_deleted &&
                    strcmp(rec.name, s_rec->name) == 0)))
    ++idx;
  close(f);
  if(found)
  {
    s_rec->area = rec.area;
    s_rec->cnt = rec.cnt;
    s_rec->is_deleted = rec.is_deleted;
  }
  return (found ? idx : -1);
}

const char* area_name(int idx)
{
  return (idx > (int)sizeof(areas) / (int)sizeof(areas[0]))
         ? NULL
         : areas[idx - 1].name;
}

int area_inspector(int idx)
{
  return (idx > (int)sizeof(areas) / (int)sizeof(areas[0]))
          ? -1
          : areas[idx - 1].inspector;
}
