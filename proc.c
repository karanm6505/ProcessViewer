#include "proc.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int is_numeric(const char *str) {
    for (size_t i = 0; str[i]; ++i) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

ssize_t get_processes(Process *list, size_t max) {
    DIR *dir = opendir("/proc");
    if (!dir) return -1;

    struct dirent *entry;
    ssize_t count = 0;

    while ((entry = readdir(dir)) != NULL && (size_t)count < max) {
        if (entry->d_type != DT_DIR) continue;
        if (!is_numeric(entry->d_name)) continue;

        pid_t pid = (pid_t)atoi(entry->d_name);
        char path[512];  // Increased buffer size
        snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

        FILE *f = fopen(path, "r");
        if (!f) continue;

        Process proc = {0};
        proc.pid = pid;

        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "Name:", 5) == 0) {
                sscanf(line, "Name:\t%255s", proc.name);
            } else if (strncmp(line, "VmRSS:", 6) == 0) {
                sscanf(line, "VmRSS: %zu", &proc.memory_kb);
            }
        }

        fclose(f);
        list[count++] = proc;
    }

    closedir(dir);
    return count;
}
