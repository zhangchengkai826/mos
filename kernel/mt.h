#ifndef _MT_H
#define _MT_H

#define MAX_TASKS 100
#define TASK_GDT0 3
#define TASK_ALLOC 1
#define TASK_RUNNING 2

struct MEMMAN;

struct TSS32 {
  int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
  int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  int es, cs, ss, ds, fs, gs;
  int ldtr, iomap;
};

struct TASK {
  int sel, flags; 
  struct TSS32 tss;
};

struct TASKCTL {
  int running; 
  int now; 
  struct TASK *tasks[MAX_TASKS];
  struct TASK tasks0[MAX_TASKS];
};

struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc();
void task_run(struct TASK *task);
void task_switch();
void task_sleep(struct TASK *task);


#endif /*_MT_H */
