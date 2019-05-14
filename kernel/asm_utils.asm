[bits 32]

global io_in8, io_out8, io_load_eflags, io_store_eflags
global asm_inthandler21, asm_inthandler2c
global load_idtr

extern inthandler21, inthandler2c

io_in8:
        mov edx, [esp+4]
        mov eax, 0
        in al, dx
        ret

io_out8:
        mov edx, [esp+4]
        mov al, [esp+8]
        out dx, al
        ret

io_load_eflags:
        pushfd
        pop eax
        ret

io_store_eflags:
        mov eax, [esp+4]
        push eax
        popfd
        ret

asm_inthandler21:
        push es
        push ds
        pushad
        mov eax, esp
        push eax
        mov ax, ss
        mov ds, ax
        mov es, ax

        call inthandler21

        pop eax
        popad
        pop ds
        pop es
        iret

asm_inthandler2c:
        push es
        push ds
        pushad
        mov eax, esp
        push eax
        mov ax, ss
        mov ds, ax
        mov es, ax

        call inthandler2c

        pop eax
        popad
        pop ds
        pop es
        iret

load_idtr:
        mov ax, [esp+4]
        mov [esp+6], ax
        lidt [esp+6]
        ret

