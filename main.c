#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include "proc.h"

#define MAX_PROCESSES 1024
#define NAME_WIDTH 25

int cmp_memory(const void *a, const void *b) {
    const Process *p1 = a;
    const Process *p2 = b;
    if (p1->memory_kb > p2->memory_kb) return -1;
    else if (p1->memory_kb < p2->memory_kb) return 1;
    return 0;
}

int main(void) {
    Process procs[MAX_PROCESSES];
    ssize_t n;

    initscr();
    noecho();
    curs_set(FALSE);

    while (1) {
        clear();

        n = get_processes(procs, MAX_PROCESSES);
        if (n < 0) {
            mvprintw(0, 0, "Error reading processes");
            refresh();
            sleep(2);
            break;
        }

        if (n > 1) qsort(procs, n, sizeof(Process), cmp_memory);

        mvprintw(0, 0, "%-8s %-*s %12s", "PID", NAME_WIDTH, "NAME", "MEMORY (KB)");
        for (ssize_t i = 0; i < n && i < LINES - 1; i++) {
            char name[NAME_WIDTH + 1];
            snprintf(name, sizeof(name), "%.*s", NAME_WIDTH, procs[i].name);
            mvprintw(i + 1, 0, "%-8d %-*s %12zu", procs[i].pid, NAME_WIDTH, name, procs[i].memory_kb);
        }

        refresh();
        sleep(1);
    }

    endwin();
    return 0;
}
