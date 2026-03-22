

GCC_COMPILER=/home/haidir/opt/cross/bin/x86_64-elf-gcc
LD_COMPILER=/home/haidir/opt/cross/bin/x86_64-elf-ld
GNU_AS_COMPILER=/home/haidir/opt/cross/bin/x86_64-elf-as
ASM_COMPILER=nasm
CC_FLAGS=-m64 -Wall -Wextra -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -fno-pie
NASM_FLAGS=-f elf64
INCLUDEPATH=-I"kernel"


KERNEL_DIR=./kernel
BUILD_DIR=./build
BOOT_DIR=./iso/boot
FILE_IMAGE=./os.img

BUILD_BOOT_DIR=${BUILD_DIR}/boot
BUILD_BOOTSTAGE_DIR=${BUILD_BOOT_DIR}/stage
BUILD_BOOTSTAGE64_DIR=${BUILD_BOOT_DIR}/stage64
BOOTLOADER_FILENAME=bootLoader.bin

# Kernel Files
KERNEL_ASM_FILES=$(shell find ${KERNEL_DIR} -name *.asm)
KERNEL_GNU_ASM_FILES=$(shell find ${KERNEL_DIR} -name *.S)
KERNEL_C_FILES=$(shell find ${KERNEL_DIR} -name *.c)

KERNEL_BUILD_DIRS=$(patsubst ./%, ${BUILD_DIR}/%, $(sort $(dir $(wildcard ${KERNEL_DIR}/*/ ))))
KERNEL_ASM_BUILD=$(patsubst ./%.asm, ${BUILD_DIR}/%.asm.o, ${KERNEL_ASM_FILES})
KERNEL_GNU_ASM_BUILD=$(patsubst ./%.S, ${BUILD_DIR}/%.gas.o, ${KERNEL_GNU_ASM_FILES})
KERNEL_C_BUILD=$(patsubst ./%.c, ${BUILD_DIR}/%.c.o, ${KERNEL_C_FILES})

all: kernel_build \
	 create_boot \
	 create_image

test:
	echo $(KERNEL_ASM_FILES)
	echo $(KERNEL_C_FILES)

kernel_build: mkdir ${KERNEL_BUILD_DIRS} ${KERNEL_ASM_BUILD} ${KERNEL_GNU_ASM_BUILD} ${KERNEL_C_BUILD}
	$(LD_COMPILER) -g -m elf_x86_64 -T ${KERNEL_DIR}/kernel.linker.ld \
		-o ${BUILD_DIR}/kernel/kernel.elf \
		${KERNEL_GNU_ASM_BUILD} ${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD}
	$(LD_COMPILER) -g -m elf_x86_64 -T ${KERNEL_DIR}/kernel.linker.ld \
		-o ${BUILD_DIR}/kernel/kernel.bin \
		${KERNEL_GNU_ASM_BUILD} ${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD} \
		--oformat=binary

${BUILD_DIR}/kernel/%.c.o: ${KERNEL_DIR}/%.c
	$(GCC_COMPILER) -g ${CC_FLAGS} ${INCLUDEPATH} -mcmodel=kernel -o $@ -c $< 

${BUILD_DIR}/kernel/%.asm.o: ${KERNEL_DIR}/%.asm
	$(ASM_COMPILER) ${NASM_FLAGS} $< -o $@

$(BUILD_DIR)/kernel/%.gas.o: ${KERNEL_DIR}/%.S
	${GNU_AS_COMPILER} --64 -o $@ $<

# Build Bootloader
create_boot:
	make -C boot


create_image:
	dd if=/dev/zero of=${FILE_IMAGE} bs=512 count=131072
	mkfs.fat -F 16 -R 128 ${FILE_IMAGE}
	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=${FILE_IMAGE} count=3 bs=1 conv=notrunc
	dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=${FILE_IMAGE} count=450 seek=62 skip=62 bs=1 conv=notrunc
	dd if=${BUILD_BOOTSTAGE64_DIR}/bootstage.bin of=${FILE_IMAGE} bs=512 seek=1 conv=notrunc
	mcopy -i ${FILE_IMAGE} ./text.txt "::text.txt"
	mcopy -i ${FILE_IMAGE} ${BUILD_DIR}/kernel/kernel.bin "::kernel.bin"
	mcopy -i ${FILE_IMAGE} ${BUILD_DIR}/kernel/kernel.elf "::kernel.elf"
	mcopy -i ${FILE_IMAGE} programs/shell/main.elf "::main.elf"

mkdir: clean
	mkdir -p build/${KERNEL_DIR}
	mkdir -p build/boot

${BUILD_DIR}/%: %
	mkdir -p $@

clean:
	rm -rf build

run:
	qemu-system-x86_64 -hda ${FILE_IMAGE} -serial stdio

run_debug:
	qemu-system-x86_64 -hda $(FILE_IMAGE) -serial stdio -S -s

run_gdb:
	gdb build/kernel/kernel.elf



bootstage_debug:
	gdb --command=debug/gdb_bootstage64.gdb
kernel_debug:
	gdb --command=debug/gdb_kernel.gdb




