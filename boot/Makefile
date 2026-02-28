
CCFLAGS=-m32 -Wextra -Wall -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -fno-pie


BUILD_BOOT_DIR=../build/boot
BUILD_BOOTSTAGE_DIR=../build/boot/stage
BOOTLOADER_FILENAME=bootLoader.bin

BOOTSTAGE_C_FILES=$(shell find ./stage/ -name '*.c')
BOOTSTAGE_ASM_FILES=$(shell find ./stage/ -name '*.asm')

BOOTSTAGE_C_OBJ=$(patsubst ./stage/%.c, ${BUILD_BOOTSTAGE_DIR}/%.c.o, ${BOOTSTAGE_C_FILES})
BOOTSTAGE_ASM_OBJ=$(patsubst ./stage/%.asm, ${BUILD_BOOTSTAGE_DIR}/%.asm.o, ${BOOTSTAGE_ASM_FILES})

all: 	build_bootloader \
		build_bootstage

list:
	echo ${BOOTSTAGE_C_FILES}
	echo ${BOOTSTAGE_ASM_FILES}
	echo ${BOOTSTAGE_C_OBJ}
	echo ${BOOTSTAGE_ASM_OBJ}

build_bootloader: mkdir
	nasm -f bin boot.asm -o ${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME}

build_bootstage: ${BOOTSTAGE_C_OBJ} ${BOOTSTAGE_ASM_OBJ}
	ld -m elf_i386 -T bootstage.linker.ld -o ${BUILD_BOOTSTAGE_DIR}/bootstage.bin ${BOOTSTAGE_ASM_OBJ} ${BOOTSTAGE_C_OBJ} --oformat=binary

${BUILD_BOOTSTAGE_DIR}/%.c.o: ./stage/%.c
	gcc ${CCFLAGS} -I"./stage/" -o $@ -c $<

${BUILD_BOOTSTAGE_DIR}/%.asm.o: ./stage/%.asm
	nasm -f elf -o $@ $<


bind:
	dd if=/dev/zero of=../OS.bin bs=512 count=131072
	mkfs.fat -F 16 -R 128 ../OS.bin
	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=../OS.bin count=3 bs=1 conv=notrunc
	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=../OS.bin count=450 seek=62 skip=62 bs=1 conv=notrunc
	dd if=${BUILD_BOOTSTAGE_DIR}/bootstage.bin of=../OS.bin bs=512 seek=1 conv=notrunc
	mcopy -i ../OS.bin ../text.txt "::text.txt"
# 	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=../OS.bin bs=512 conv=notrunc

mkdir:
	mkdir -p 	${BUILD_BOOT_DIR}
	mkdir -p 	${BUILD_BOOTSTAGE_DIR}
	mkdir -p 	${BUILD_BOOTSTAGE_DIR}/driver \
				${BUILD_BOOTSTAGE_DIR}/fonts \
				${BUILD_BOOTSTAGE_DIR}/cpu

run_test:
	qemu-system-x86_64 -hda ../OS.bin -serial stdio