#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include "proc.h"

#define MAX_PROCESSES 1024

int main()
{
  Process procs[MAX_PROCESSES];

  initscr();
  noecho();
  curs_set(FALSE);

  while(1){
    clear();

    ssize_t n = get_processes(procs, MAX_PROCESSES);
    if(n < 0)
    {
      mvprintw(0, 0, "Error reading processes:");
      refresh();
      sleep(2);
      break;
    }

    mvprintw(0, 0, "PID  NAME       MEMORY(KB)");


    for(ssize_t i = 0; i < n && i < LINES-1; i++){
      mvprintw(i+1, 0, "%-7d %-20s %8zu", procs[i].pid, procs[i].name, procs[i].memory_kb);

    }

    refresh();
    sleep(1);
  }
  
  endwin();
  return 0;
}
