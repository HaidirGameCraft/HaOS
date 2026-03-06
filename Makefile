

CC_FLAGS=-m32 -Wall -Wextra -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -fno-pie
NASM_FLAGS=-f elf
INCLUDEPATH=-I"kernel"


KERNEL_DIR=./kernel
BUILD_DIR=./build
BOOT_DIR=./iso/boot
FILE_IMAGE=./os.img

BUILD_BOOT_DIR=${BUILD_DIR}/boot
BUILD_BOOTSTAGE_DIR=${BUILD_BOOT_DIR}/stage
BOOTLOADER_FILENAME=bootLoader.bin

# Kernel Files
KERNEL_ASM_FILES=$(shell find ${KERNEL_DIR} -name *.asm)
KERNEL_C_FILES=$(shell find ${KERNEL_DIR} -name *.c)

KERNEL_BUILD_DIRS=$(patsubst ./%, ${BUILD_DIR}/%, $(sort $(dir $(wildcard ${KERNEL_DIR}/*/ ))))
KERNEL_ASM_BUILD=$(patsubst ./%.asm, ${BUILD_DIR}/%.asm.o, ${KERNEL_ASM_FILES})
KERNEL_C_BUILD=$(patsubst ./%.c, ${BUILD_DIR}/%.c.o, ${KERNEL_C_FILES})

all: kernel_build \
	 create_boot \
	 create_image

test:
	echo $(KERNEL_ASM_FILES)
	echo $(KERNEL_C_FILES)

kernel_build: mkdir ${KERNEL_BUILD_DIRS} ${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD}
	ld -m elf_i386 -T ${KERNEL_DIR}/kernel.linker.ld \
		-o ${BUILD_DIR}/kernel/kernel.bin \
		${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD} \
		--oformat binary
	ld -g -m elf_i386 -T ${KERNEL_DIR}/kernel.linker.ld \
		-o ${BUILD_DIR}/kernel/kernel.elf \
		${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD}
	

${BUILD_DIR}/kernel/%.c.o: ${KERNEL_DIR}/%.c
	gcc ${CC_FLAGS} ${INCLUDEPATH} -o $@ -c $< 

${BUILD_DIR}/kernel/%.asm.o: ${KERNEL_DIR}/%.asm
	nasm ${NASM_FLAGS} $< -o $@

# Build Bootloader
create_boot:
	make -C boot


create_image:
	dd if=/dev/zero of=${FILE_IMAGE} bs=512 count=131072
	mkfs.fat -F 16 -R 128 ${FILE_IMAGE}
	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=${FILE_IMAGE} count=3 bs=1 conv=notrunc
	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=${FILE_IMAGE} count=450 seek=62 skip=62 bs=1 conv=notrunc
	dd if=${BUILD_BOOTSTAGE_DIR}/bootstage.bin of=${FILE_IMAGE} bs=512 seek=1 conv=notrunc
	mcopy -i ${FILE_IMAGE} ./text.txt "::text.txt"
	mcopy -i ${FILE_IMAGE} ${BUILD_DIR}/kernel/kernel.bin "::kernel.bin"

mkdir: clean
	mkdir -p build/${KERNEL_DIR}
	mkdir -p build/boot

${BUILD_DIR}/%: %
	mkdir -p $@

clean:
	rm -rf build

run:
	qemu-system-i386 -hda ${FILE_IMAGE} -serial stdio

run_debug:
	qemu-system-i386 -hda $(FILE_IMAGE) -serial stdio -S -s

run_gdb:
	gdb build/kernel/kernel.elf








