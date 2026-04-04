
UEFI_BUILD_DIR=../../build/UEFI
GCC_FLAGS=-fno-stack-protector \
			-fpic -fshort-wchar \
			-mno-red-zone \
			-I/usr/include/efi/ -I/usr/include/efi/x86_64 \
			-DEFI_FUNCTION_WRAPPER
LD_FLAGS=	-g \
			-nostdlib \
			-znocombreloc \
			-T /usr/lib/elf_x86_64_efi.lds \
			-shared \
			-Bsymbolic \
			-L /lib/ -lgnuefi -lefi
C_EFI_FILES=$(shell find . -name '*.c' )
C_EFI_OBJECT_FILES=$(patsubst ./%.c, ${UEFI_BUILD_DIR}/%.c.o, ${C_EFI_FILES})
ASM_EFI_FILES=$(shell find . -maxdepth 1 -name '*.s' )
ASM_EFI_OBJECT_FILES=$(patsubst ./%.s, ${UEFI_BUILD_DIR}/%.s.o, ${ASM_EFI_FILES})

build_uefi: mk_dir build_go_to_kernel ${ASM_EFI_OBJECT_FILES} ${C_EFI_OBJECT_FILES}
	@echo "Linking UEFI program..."
	ld /lib/crt0-efi-x86_64.o ${C_EFI_OBJECT_FILES} ${ASM_EFI_OBJECT_FILES} ${LD_FLAGS} -o ${UEFI_BUILD_DIR}/uefi.so
	objcopy -j .text \
			-j .rodata \
			-j .data \
			-j .bss \
			-j .dynamic \
			-j .dynsym \
			-j .rel \
			-j .rela \
			-j .reloc --output-target=efi-app-x86_64 ${UEFI_BUILD_DIR}/uefi.so ${UEFI_BUILD_DIR}/haosboot.efi
${UEFI_BUILD_DIR}/%.c.o: ./%.c
	gcc -g ${GCC_FLAGS} -I"../../kernel" -o $@ -c $<
${UEFI_BUILD_DIR}/%.s.o: ./%.s
	as --64 -o $@ $<

build_go_to_kernel:
	as --64 -o $(UEFI_BUILD_DIR)/go_to_kernel/kernel.o ./go_to_kernel/go_to_kernel.s
	ld $(UEFI_BUILD_DIR)/go_to_kernel/kernel.o -m elf_x86_64 -Ttext 0x15000 -o $(UEFI_BUILD_DIR)/go_to_kernel/kernel.bin --oformat=binary

mk_dir:
	mkdir -p ${UEFI_BUILD_DIR}/go_to_kernel
