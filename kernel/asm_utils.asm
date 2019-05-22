[bits 32]

global io_in8, io_in32, io_out8, io_load_eflags, io_store_eflags
global asm_inthandler20, asm_inthandler21, asm_inthandler2c
global load_gdtr, load_idtr, load_tr, load_cr0, store_cr0, memtest_sub
global taskswitch4

extern inthandler20, inthandler21, inthandler2c

taskswitch4:
        jmp 4*8:0
        ret

io_in8:
        mov edx, [esp+4]
        mov eax, 0
        in al, dx
        ret

io_in32:
        mov edx, [esp+4]
        in eax, dx
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

asm_inthandler20:
        push es
        push ds
        pushad
        mov eax, esp
        push eax
        mov ax, ss
        mov ds, ax
        mov es, ax

        call inthandler20

        pop eax
        popad
        pop ds
        pop es
        iret

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

load_gdtr:
        mov ax, [esp+4]
        mov [esp+6], ax
        lgdt [esp+6]
        ret

load_idtr:
        mov ax, [esp+4]
        mov [esp+6], ax
        lidt [esp+6]
        ret

load_tr:
        ltr [esp+4]
        ret

load_cr0:
        mov eax, cr0
        ret

store_cr0:
        mov eax, [esp+4]
        mov cr0, eax
        ret

memtest_sub:
        push edi
        push esi
        push ebx
        mov esi, 0xaa55aa55
        mov edi, 0x55aa55aa
        mov eax, [esp+12+4]
mts_loop:
        mov ebx, eax
        add ebx, 0xffc
        mov edx, [ebx]
        mov [ebx], esi
        xor dword [ebx], 0xffffffff
        cmp edi, [ebx]
        jne mts_fin
        xor dword [ebx], 0xffffffff
        cmp esi, [ebx]
        jne mts_fin
        mov [ebx], edx
        add eax, 0x1000
        cmp eax, [esp+12+8]
        jbe mts_loop
        pop ebx
        pop esi
        pop edi
        ret
mts_fin:
        mov [ebx], edx
        pop ebx
        pop esi
        pop edi
        ret

