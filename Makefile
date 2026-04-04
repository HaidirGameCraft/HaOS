

GCC_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-gcc
LD_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-ld
GNU_AS_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-as
ASM_COMPILER=nasm
CC_FLAGS=-m64 -Wall -Wextra -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -fno-pie
NASM_FLAGS=-f elf64
INCLUDEPATH=-I"kernel"


KERNEL_DIR=./kernel
BUILD_DIR=./build
BOOT_DIR=./iso/boot
FILE_IMAGE=./os.img
GPT_FILE_IMAGE=./os.gpt.img

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

all: gpt_part
# all: kernel_build \
# 	 create_boot \
# 	 create_image

test:
	@echo $(KERNEL_ASM_FILES)
	@echo $(KERNEL_C_FILES)

gpt_part: mkdir kernel_build
	make create_boot
	make create_gptImage

kernel_build: ${KERNEL_BUILD_DIRS} ${KERNEL_ASM_BUILD} ${KERNEL_GNU_ASM_BUILD} ${KERNEL_C_BUILD}
	@$(LD_COMPILER) -g -m elf_x86_64 -T ${KERNEL_DIR}/kernel.linker.ld \
		-o ${BUILD_DIR}/kernel/kernel.elf \
		${KERNEL_GNU_ASM_BUILD} ${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD}
	@$(LD_COMPILER) -g -m elf_x86_64 -T ${KERNEL_DIR}/kernel.linker.ld \
		-o ${BUILD_DIR}/kernel/kernel.bin \
		${KERNEL_GNU_ASM_BUILD} ${KERNEL_ASM_BUILD} ${KERNEL_C_BUILD} \
		--oformat=binary

${BUILD_DIR}/kernel/%.c.o: ${KERNEL_DIR}/%.c
	$(GCC_COMPILER) -g ${CC_FLAGS} ${INCLUDEPATH} -o $@ -c $< -mcmodel=kernel 

${BUILD_DIR}/kernel/%.asm.o: ${KERNEL_DIR}/%.asm
	$(ASM_COMPILER) ${NASM_FLAGS} $< -o $@

$(BUILD_DIR)/kernel/%.gas.o: ${KERNEL_DIR}/%.S
	${GNU_AS_COMPILER} --64 -o $@ $<

# Build Bootloader
create_boot:
	@make -C boot


create_image:
	@dd if=/dev/zero of=${FILE_IMAGE} bs=512 count=131072
	@mkfs.fat -F 16 -R 128 ${FILE_IMAGE}
	@dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=${FILE_IMAGE} count=3 bs=1 conv=notrunc
	@dd if=${BUILD_BOOT_DIR}/${BOOTLOADER_FILENAME} of=${FILE_IMAGE} count=450 seek=62 skip=62 bs=1 conv=notrunc
	@dd if=${BUILD_BOOT_DIR}/boot32.bin of=${FILE_IMAGE} bs=512 seek=1 conv=notrunc
	@dd if=${BUILD_BOOTSTAGE64_DIR}/bootstage.bin of=${FILE_IMAGE} bs=512 seek=2 conv=notrunc
	@mcopy -i ${FILE_IMAGE} ./text.txt "::text.txt"
	@mcopy -i ${FILE_IMAGE} ./font.map "::font.map"
	@mcopy -i ${FILE_IMAGE} ${BUILD_DIR}/kernel/kernel.bin "::kernel.bin"
	@mcopy -i ${FILE_IMAGE} ${BUILD_DIR}/kernel/kernel.elf "::kernel.elf"
	@mcopy -i ${FILE_IMAGE} programs/shell/main.elf "::main.elf"

create_gptImage:
	@echo "Creating GPT Disk"
	@dd if=/dev/zero of=${GPT_FILE_IMAGE} bs=1M count=128
	parted ${GPT_FILE_IMAGE} --script mklabel gpt
	parted ${GPT_FILE_IMAGE} --script mkpart ESP fat16 1MiB 64MiB
	parted ${GPT_FILE_IMAGE} --script set 1 esp off
	mkfs.fat -F16 --offset=2048 ${GPT_FILE_IMAGE}
	mmd -i ${GPT_FILE_IMAGE}@@1M ::/EFI
	mmd -i ${GPT_FILE_IMAGE}@@1M ::/EFI/BOOT
	mcopy -i ${GPT_FILE_IMAGE}@@1M build/UEFI/haosboot.efi ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i ${GPT_FILE_IMAGE}@@1M ${BUILD_DIR}/kernel/kernel.bin ::/kernel.bin
	mcopy -i ${GPT_FILE_IMAGE}@@1M build/UEFI/go_to_kernel/kernel.bin ::/gkrnl.bin
	@mcopy -i ${GPT_FILE_IMAGE}@@1M ./font.map "::font.map"
	mdir -i ${GPT_FILE_IMAGE}@@1M ::

mkdir: clean
	@mkdir -p build/${KERNEL_DIR}
	@mkdir -p build/boot

${BUILD_DIR}/%: %
	@mkdir -p $@

clean:
	@rm -rf build

run_uefi:
	qemu-system-x86_64 -drive format=raw,file=${GPT_FILE_IMAGE} -bios /usr/share/OVMF/OVMF_CODE.fd -d in_asm,int -D system-uefi-logs.txt -serial stdio

run:
	bochs -f bochsrc
#	qemu-system-x86_64 -hda ${FILE_IMAGE} -serial stdio -d in_asm,int -D system-logs.txt -no-shutdown -no-reboot

run_debug:
	qemu-system-x86_64 -hda $(FILE_IMAGE) -serial stdio -S -s
run_debug_uefi:
	qemu-system-x86_64 -drive format=raw,file=${GPT_FILE_IMAGE} -bios /usr/share/OVMF/OVMF_CODE.fd -d in_asm,int -D system-uefi-logs.txt -serial stdio -S -s

run_gdb:
	gdb build/kernel/kernel.elf



bootstage_debug:
	gdb --command=debug/gdb_bootstage64.gdb
kernel_debug:
	gdb --command=debug/gdb_kernel.gdb
uefi_debug:
	gdb --command=debug/gdb_uefi.gdb

# Burning to USB Drive ( Test for Read Machine ), Dont Do that on your real machine
burn_usb:
	@if [ -z "$(TARGET)" ]; then \
		echo "Error: FILE is not set to burn"; \
		exit 1; \
	fi
	make
	@echo "Burning USB"
	sudo dd if=/dev/zero of=$(TARGET) bs=512 count=131072
	sudo dd if=os.img of=$(TARGET) bs=512 status=progress conv=notrunc
#	sync
	@echo "Burning USB is Completed"
	
	
	
	
	
	









