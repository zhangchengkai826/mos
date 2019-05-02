os-image.bin: kernel.bin boot_sect.bin
	cat boot_sect.bin kernel.bin > os-image.bin

kernel.bin: kernel.o
	ld -o kernel.bin -Ttext 0x1000 kernel.o --oformat binary

kernel.o:
	gcc -ffreestanding -c kernel.c -o kernel.o

clean:
	rm *.bin *.o 

boot_sect.bin: boot_sect.asm
	nasm boot_sect.asm -f bin -o boot_sect.bin

