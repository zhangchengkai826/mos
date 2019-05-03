[bits 32]

global io_out8, io_load_eflags, io_store_eflags

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

