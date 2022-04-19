#include <stdio.h>
#include <stdbool.h>

#include "db.h"

#define L_AREA 20

#define STR_(X) #X
#define STR(X) STR_(X)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char menu(void);
void new_reg(void);
void mod_reg(void);
void del_reg(void);
void print_list(bool);
int print_rec(int, Bunny*);
void inp_name(char*);
void inp_area(int*);
void inp_cnt(int*);

int main()
{
  printf("\e[1;1H\e[2J");
  printf("Husveti Locsolokiraly valasztas\n");
  printf("-------------------------------\n");
  char choice;
  while ((choice = menu()) != 'k')
  {
    switch (choice)
    {
      case '1' : new_reg(); break;
      case '2' : mod_reg(); break;
      case '3' : del_reg(); break;
      case '4' : print_list(false); break;
      case '5' : print_list(true); break;
      case '6' : printf("/nTodo.../n"); break;
      default : fprintf(stderr,"Hiba: ervenytelen valasztas\n"); break;
    }
  }
  return 0;
}

char menu()
{
  printf("\nValassz az alabbi lehetosegek kozul:\n");
  printf("\n(1) Uj regisztracio");
  printf("\n(2) Regisztracio modositasa");
  printf("\n(3) Regisztracio torlese");
  printf("\n(4) Osszes jelentkezo listaja");
  printf("\n(5) Jelentkezok listaja lakoteruletenkent");
  printf(ANSI_COLOR_YELLOW);
  printf("\n(6) Verseny indítása\n");
  printf(ANSI_COLOR_RESET);
  printf("\nvagy (k)ilepes a programbol? ");
  char choice = getchar();
  while (getchar() != '\n');
  return choice;
}

void new_reg()
{
  Bunny rec;
  printf("\nUj regisztracio");
  printf("\n---------------\n");
  inp_name(rec.name);
  if (-1 == src(&rec))
  {
    inp_area(&rec.area);
    inp_cnt(&rec.cnt);
    rec.is_deleted = false;
    ins(&rec);
    printf("Eredmeny: %s rogzitese sikeres volt\n", rec.name);    
  }
  else
  {
    fprintf(stderr,"Hiba: %s nevu nyuszi mar regisztralt\n", rec.name);
  }
}

void mod_reg()
{
  Bunny rec;
  int idx;
  printf("\nRregisztracio modositasa");
  printf("\n------------------------\n");
  inp_name(rec.name);
  if (-1 == (idx = src(&rec)))
  {
    fprintf(stderr,"Hiba: nincs %s neven regisztralt nyuszi\n", rec.name);
  }
  else
  {
    printf("Jelenlegi lakoterulet: %s\n", area_name(rec.area));
    printf("Jelenlegi jelentkezesek szama: %i\n", rec.cnt);
    inp_area(&rec.area);
    inp_cnt(&rec.cnt);
    upd(idx, &rec);
    printf("Eredmeny: %s modositasa sikeres volt\n", rec.name);    
  } 
}

void del_reg()
{
  Bunny rec;
  int idx;
  printf("\nRegisztracio torlese");
  printf("\n--------------------\n");
  inp_name(rec.name);
  if (-1 == (idx = src(&rec)))
  {
    fprintf(stderr,"Hiba: nincs %s neven regisztralt nyuszi\n", rec.name);
  }
  else
  {
    rec.is_deleted = true;
    upd(idx, &rec);
    printf("Eredmeny: %s torlese sikeres volt\n", rec.name);    
  }
}

void print_list(bool is_filtered)
{
  int area = 0;
  if (is_filtered)
  {
    printf("\nJelentkezok listaja lakoteruletenkent");
    printf("\n-------------------------------------\n");
    inp_area(&area);
  }
  else
  {
    printf("\nOsszes jelentkezo listaja");
    printf("\n-------------------------\n");
  }
  printf("\n %-" STR(L_NAME) "s| %-" STR(L_AREA) "s| Jelentkezesek szama\n",
          "Nyuszi neve", "Lakoterulet");
  for(int i = 0; i < 25 + L_NAME + L_AREA; ++i) printf("-");
  printf("\n");
  if(0 == sel(area, &print_rec))
    printf("Nincs megjelenitheto adat\n");
}

int print_rec(int area, Bunny* rec)
{
  int c = 0;
  if (0 == area || rec->area == area)
  {
    printf(" %-" STR(L_NAME) "s| %-" STR(L_AREA) "s| %19i\n",
             rec->name, area_name(rec->area), rec->cnt);
    c = 1;
  }
  return c;
}

void inp_name(char* name)
{
  printf("\nNyuszi neve (max. " STR(L_NAME) " karakter)? ");
  scanf("%" STR(L_NAME) "[^\n]s", name);
  while (getchar() != '\n');
}

void inp_area(int* area)
{
  bool is_invalid;
  do
  {
    int i = 1;
    printf("\n");
    while(NULL != area_name(i)) {printf("(%i)%s ", i, area_name(i)); ++i;}
    printf("\nLakoterulet sorszama (valassz a fenti lehetosegek kozul)? ");
    if ((is_invalid = (scanf("%i", area) == 0 || *area < 1 || *area > i - 1)))
      fprintf(stderr, "Hiba: ervenytelen adat\n");
    while (getchar() != '\n');
  } while (is_invalid);
}

void inp_cnt(int* cnt)
{
  bool is_invalid;
  do
  {
    printf("\nHanyadik alkalommal jelentkezik? ");
    if ((is_invalid = (scanf("%i", cnt) == 0 || *cnt < 1)))
      fprintf(stderr, "Hiba: ervenytelen adat\n");
    while (getchar() != '\n');
  } while (is_invalid);
}
