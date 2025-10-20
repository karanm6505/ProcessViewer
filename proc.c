#include "proc.h"

#ifdef __APPLE__  // macOS
#include <libproc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssize_t get_processes(Process *list, size_t max) {
    int num_pids = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    if (num_pids <= 0) return -1;

    pid_t *pids = malloc(num_pids * sizeof(pid_t));
    if (!pids) return -1;

    num_pids = proc_listpids(PROC_ALL_PIDS, 0, pids, num_pids * sizeof(pid_t));
    if (num_pids <= 0) {
        free(pids);
        return -1;
    }

    ssize_t count = 0;
    for (int i = 0; i < num_pids && (size_t)count < max; i++) {
        pid_t pid = pids[i];
        if (pid == 0) continue;

        struct proc_taskinfo ti;
        int ret = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &ti, sizeof(ti));
        if (ret <= 0) continue;

        char name[256];
        ret = proc_name(pid, name, sizeof(name));
        if (ret <= 0) continue;

        Process p = {0};
        p.pid = pid;
        strncpy(p.name, name, sizeof(p.name)-1);
        p.memory_kb = ti.pti_resident_size / 1024;

        list[count++] = p;
    }

    free(pids);
    return count;
}

#elif __linux__  // Linux
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
        char path[512];
        snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

        FILE *f = fopen(path, "r");
        if (!f) continue;

        Process proc = {0};
        proc.pid = pid;

        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "Name:", 5) == 0) {
                sscanf(line, "Name:\t%256s", proc.name);
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

#else
#error "Unsupported OS"
#endif

