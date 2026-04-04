
<<<<<<< HEAD
=======
LD_LINKER=${HOME}/opt/cross/bin/x86_64-elf-ld
>>>>>>> 8029d96 (HaOS 64 bits Updated)
GCC_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-gcc

build:
	$(GCC_COMPILER) -g -m64 -e main -ffreestanding -fno-pie -fno-plt -Wall -Wextra -nostdlib -fno-builtin -I"../../kernel" -c  ./main.c -o ./main.o
	${LD_LINKER} -g -e main -m elf_x86_64 ./main.o -L../../shared -lc -o ./main.elf
