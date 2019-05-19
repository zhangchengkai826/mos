#ifndef _TIMER_H
#define _TIMER_H

void init_pit();
void asm_inthandler20();
void inthandler20(int *esp);

#endif /* _TIMER_H */
