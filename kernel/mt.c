#include "mt.h"
#include "mem.h"

struct TIMER *task_timer;

struct TASK *task_init(struct MEMMAN *memman) {
  int i;
  struct TASK *task;
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)GDT_ADDR;
  *PTASKCTL_ADDR = (struct TASKCTL *)memman_alloc_4k(memman, sizeof (struct TASKCTL));
  struct taskctl = *PTASKCTL_ADDR;
  *PTASKTIMER_ADDR = timer_alloc();
  struct TIMER *task_timer = (struct TIMER *)*PTASKTIMER_ADDR;
  for (i = 0; i < MAX_TASKS; i++) {
    taskctl->tasks0[i].flags = 0;
    taskctl->tasks0[i].sel = (TASK_GDT0 + i) << 3;
    set_segmdesc(gdt + TASK_GDT0 + i, 103, (int)&taskctl->tasks0[i].tss, AR_TSS32);
  }
  task = task_alloc();
  task->flags = TASK_RUNNING;
  taskctl->running = 1;
  taskctl->now = 0;
  taskctl->tasks[0] = task;
  load_tr(task->sel);
  timer_settime(task_timer, 2);
  return task;
}

struct TASK *task_alloc() {
  int i;
  struct TASK *task;
  struct taskctl = *PTASKCTL_ADDR;
  for (i = 0; i < MAX_TASKS; i++) {
    if (taskctl->tasks0[i].flags == 0) {
      task = &taskctl->tasks0[i];
      task->flags = TASK_ALLOC;
      task->tss.eflags = 0x00000202;
      task->tss.eax = 0;
      task->tss.ecx = 0;
      task->tss.edx = 0;
      task->tss.ebx = 0;
      task->tss.ebp = 0;
      task->tss.esi = 0;
      task->tss.edi = 0;
      task->tss.es = 0;
      task->tss.ds = 0;
      task->tss.fs = 0;
      task->tss.gs = 0;
      task->tss.ldtr = 0;
      task->tss.iomap = 0x40000000;
      return task;
    }
  }
  return 0;
}

void task_run(struct TASK *task) {
  struct taskctl = *PTASKCTL_ADDR;
  task->flags = TASK_RUNNING;
  taskctl->tasks[taskctl->running] = task;
  taskctl->running++;
}

void task_switch()
{
  struct taskctl = *PTASKCTL_ADDR;
  struct TIMER *task_timer = (struct TIMER *)*PTASKTIMER_ADDR;
  timer_settime(task_timer, 2);
  if (taskctl->running >= 2) {
    taskctl->now++;
    if (taskctl->now == taskctl->running) {
      taskctl->now = 0;
    }
    farjmp(0, taskctl->tasks[taskctl->now]->sel);
  }
}

void task_sleep(struct TASK *task)
{
  int i;
  char ts = 0;
  if (task->flags == TASK_RUNNING) {
    if (task == taskctl->tasks[taskctl->now]) {
      ts = 1; 
    }
    for (i = 0; i < taskctl->running; i++) {
      if (taskctl->tasks[i] == task) {
        break;
      }
    }
    taskctl->running--;
    if (i < taskctl->now) {
      taskctl->now--; 
    }
    for (; i < taskctl->running; i++) {
      taskctl->tasks[i] = taskctl->tasks[i + 1];
    }
    task->flags = TASK_ALLOC; 
    if (ts != 0) {
      if (taskctl->now >= taskctl->running) {
        taskctl->now = 0;
      }
      farjmp(0, taskctl->tasks[taskctl->now]->sel);
    }
  }
}

