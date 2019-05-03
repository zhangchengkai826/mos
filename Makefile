C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)

OBJ = ${C_SOURCES:.c=.o}

all: os-image.bin

os-image.bin: boot/boot_sect.bin kernel.bin
	cat $^ > $@

kernel.bin: kernel/kernel_entry.o kernel/asm_utils.o ${OBJ}
	ld -o $@ -Ttext 0x1000 $^ -m elf_i386 -s --oformat binary

%.o: %.c ${HEADERS}
	gcc -ffreestanding -c $< -o $@ -m32 -fno-pie

%.o: %.asm
	nasm $< -f elf32 -o $@

%.bin: %.asm
	nasm $< -f bin -I 'boot/' -o $@

clean:
	rm -fr *.bin *.dis *.o
	rm -fr kernel/*.o boot/*.bin drivers/*.o

