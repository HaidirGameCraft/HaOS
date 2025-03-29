I386=/opt/i386-elf/bin
GCC=$(I386)/i386-elf-gcc
LD=$(I386)/i386-elf-ld

ARCHITECTURE=x86
OUTPUT_FILE=os.img
DRIVE_NAME="Local Disk"
CFLAGS=-m32 -ffreestanding -Wall -Wextra -fno-pie -nostdlib
ifeq (${ARCHITECTURE}, x86)
	CBOOTFLAGS=-m32 -ffreestanding -Wall -Wextra -fno-pie -nostdlib
	NASMFLAGS=-f elf32
	LDBOOT=-m elf_i386
else
	CBOOTFLAGS=-m64 -ffreestanding -Wall -Wextra -fno-pie -nostdlib
	NASMFLAGS=-f elf64
	LDBOOT=-m elf_x86_64
endif

INCLUDEPATH=-Iinclude/

ASM_SOURCES=$(shell find boot/boot_stage/${ARCHITECTURE} -name "*.asm")
ASM_OBJ_SOURCES=$(patsubst boot/boot_stage/${ARCHITECTURE}/%.asm,build/boot_stage/${ARCHITECTURE}/%.o, $(ASM_SOURCES) )

C_SOURCES=$(shell find boot/boot_stage/${ARCHITECTURE} -name "*.c")
C_OBJ_SOURCES=$(patsubst boot/boot_stage/${ARCHITECTURE}/%.c,build/boot_stage/${ARCHITECTURE}/%.c.o, $(C_SOURCES) )

KERNEL_ASM_SOURCES=$(shell find kernel/ -name "*.asm")
KERNEL_ASM_OBJ_SOURCES=$(patsubst kernel/%.asm,build/kernel/%.asm.o, $(KERNEL_ASM_SOURCES) )

KERNEL_C_SOURCES=$(shell find kernel/ -name "*.c")
KERNEL_C_OBJ_SOURCES=$(patsubst kernel/%.c,build/kernel/%.c.o, $(KERNEL_C_SOURCES) )

build: mkdir
	nasm -f bin boot/boot.asm -o build/bootloader/boot.bin
	nasm -f bin boot/load_stage/load_stage.asm -o build/bootloader/load_stage.bin
	make compile_file
	make libc_compiler
	make create_fs
	make insert_file

create_fs:
	echo "Creating FileSystem..."
	dd if=/dev/zero of=$(OUTPUT_FILE) bs=512 count=131700
	mkfs.fat -F 32 -n $(DRIVE_NAME) $(OUTPUT_FILE)
	mcopy -i $(OUTPUT_FILE) build/boot_stage/${ARCHITECTURE}/boot_stage.bin "::/boots.bin"
	mcopy -i $(OUTPUT_FILE) build/kernel.bin "::/kernel.bin"
	mcopy -i $(OUTPUT_FILE) build/kernel.elf "::/kernel.elf"
	mcopy -i $(OUTPUT_FILE) tools/font/bitfont.bin "::/bitfont.bin"
	mcopy -i $(OUTPUT_FILE) text.txt "::/text_welcome.txt"
	mcopy -i $(OUTPUT_FILE) build/tty.elf "::/tty.elf"

create_test_fs:
	dd if=/dev/zero of=test.img bs=512 count=131700
	mkfs.fat -F 32 -n $(DRIVE_NAME) test.img

insert_file:
	dd if=build/bootloader/boot.bin of=$(OUTPUT_FILE) bs=512 conv=notrunc
	dd if=build/bootloader/load_stage.bin of=$(OUTPUT_FILE) bs=512 seek=2 conv=notrunc

compile_file: $(ASM_OBJ_SOURCES) $(C_OBJ_SOURCES) build_kernel
	ld $(LDBOOT) -T boot/boot_stage/link.ld -o build/boot_stage/${ARCHITECTURE}/boot_stage.bin build/boot_stage/${ARCHITECTURE}/main.o $(C_OBJ_SOURCES) --oformat binary

build/boot_stage/${ARCHITECTURE}/%.o: boot/boot_stage/${ARCHITECTURE}/%.asm
	echo "$< -> $@"
	nasm $(NASMFLAGS) $< -o $@
build/boot_stage/${ARCHITECTURE}/%.c.o: boot/boot_stage/${ARCHITECTURE}/%.c
	echo "$< -> $@"
	gcc $(CBOOTFLAGS) -c $< -o $@ $(INCLUDEPATH)

run:
	qemu-system-x86_64 -hda os.img

mkdir: clean
	mkdir build
	mkdir build/bootloader
	mkdir -p build/boot_stage/${ARCHITECTURE}
	mkdir build/kernel
clean:
	rm -rf build

build_kernel: $(KERNEL_ASM_OBJ_SOURCES) $(KERNEL_C_OBJ_SOURCES)
	ld -m elf_i386 -T kernel/kernel.ld -o build/kernel.bin build/kernel/entry.asm.o $(KERNEL_C_OBJ_SOURCES) $(KERNEL_ASM_OBJ_SOURCES) --oformat binary
	ld -m elf_i386 -T kernel/kernel.ld -o build/kernel.elf build/kernel/entry.asm.o $(KERNEL_C_OBJ_SOURCES) $(KERNEL_ASM_OBJ_SOURCES) --oformat elf32-i386

build/kernel/%.c.o: kernel/%.c
	gcc $(CFLAGS) -c $< -o $@ $(INCLUDEPATH)

build/kernel/%.asm.o: kernel/%.asm
	nasm -f elf32 $< -o $@




LIBC_C_SOURCES=$(shell find 'src/libc' -name "*.c")
LIBC_C_OBJECT=$(patsubst src/libc/%.c,build/libc/%.c.o, $(LIBC_C_SOURCES) )
LIBC_ASM_SOURCES=$(shell find src/libc -name "*.asm")
LIBC_ASM_OBJECT=$(patsubst src/libc/%.asm,build/libc/%.o, $(LIBC_ASM_SOURCES) )

libc_compiler: mkdir_libc $(LIBC_C_OBJECT) $(LIBC_ASM_OBJECT)
	make tty_compiler

mkdir_libc:
	mkdir -p build/libc

build/libc/%.c.o: src/libc/%.c
	gcc $(CFLAGS) $(INCLUDEPATH) -c $< -o $@

build/libc/%.o: src/libc/%.asm
	nasm -f elf32 $< -o $@

tty_compiler: src/tty.c
	mkdir -p build/src
	gcc $(CFLAGS) $(INCLUDEPATH) -c src/tty.c -o build/src/tty.o
	echo 'ENTRY(main)' >> build/_tty.ld
	ld -T src/tty.ld -m elf_i386 -o build/tty.elf build/src/tty.o $(LIBC_C_OBJECT) $(LIBC_ASM_OBJECT)
