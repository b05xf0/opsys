#ifndef DB_H
#define DB_H

#define L_NAME 26

typedef struct {
  char name[L_NAME + 1];
  int inspector; 
} Area;

typedef struct {
  char name[L_NAME + 1];
  bool is_deleted;
  int area, cnt;
} Bunny;

void ins(Bunny*);
void upd(int, Bunny*);
int sel(int, int (*printer)(Bunny*));
int src(Bunny*);
const char* area_name(int);
int area_inspector(int);

#endif
