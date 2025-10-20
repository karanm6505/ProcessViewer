#ifndef PROC_H
#define PROC_H

#include <sys/types.h> // for pid_t, ssize_t
#include <stdint.h> // for uint64_t
typedef struct{
  pid_t pid;
  char name[256];
  size_t memory_kb;
}Process;

#include <stddef.h>
ssize_t get_processes(Process* list, size_t max);

#endif
