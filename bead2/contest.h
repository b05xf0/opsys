#ifndef CONTEST_H
#define CONTEST_H

#include "db.h"

#define N_INSPECTOR 2

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define LOG_BOSS      ANSI_COLOR_RED"Fonyuszi"ANSI_COLOR_RESET": "
#define LOG_INSPECTOR ANSI_COLOR_GREEN"Felugyelo(%i)"ANSI_COLOR_RESET": "

void run_contest(Bunny*);

#endif