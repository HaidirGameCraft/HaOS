
OUTPUT_FILE=os.img
DRIVE_NAME="Local Disk"

ASM_SOURCES=$(shell find boot/boot_stage -name "*.asm")
ASM_OBJ_SOURCES=$(patsubst boot/boot_stage/%.asm,build/boot_stage/%.o, $(ASM_SOURCES) )

build: mkdir
	nasm -f bin boot/boot.asm -o build/bootloader/boot.bin
	nasm -f bin boot/load_stage/load_stage.asm -o build/bootloader/load_stage.bin
	make compile_file
	make create_fs
	make insert_file
	make run

create_fs:
	echo "Creating FileSystem..."
	dd if=/dev/zero of=$(OUTPUT_FILE) bs=512 count=131700
	mkfs.fat -F 32 -n $(DRIVE_NAME) $(OUTPUT_FILE)
	mcopy -i $(OUTPUT_FILE) build/boot_stage/boot_stage.bin "::/boots.bin"

create_test_fs:
	dd if=/dev/zero of=test.img bs=512 count=131700
	mkfs.fat -F 32 -n $(DRIVE_NAME) test.img

insert_file:
	dd if=build/bootloader/boot.bin of=$(OUTPUT_FILE) bs=512 conv=notrunc
	dd if=build/bootloader/load_stage.bin of=$(OUTPUT_FILE) bs=512 seek=2 conv=notrunc

compile_file: $(ASM_OBJ_SOURCES)
	ld -m elf_i386 -Ttext 0x1000 -o build/boot_stage/boot_stage.bin $(ASM_OBJ_SOURCES) --oformat binary

build/boot_stage/%.o: boot/boot_stage/%.asm
	echo "$< -> $@"
	nasm -f elf32 $< -o $@

run:
	qemu-system-i386 -hda os.img

mkdir: clean
	mkdir build
	mkdir build/bootloader
	mkdir build/boot_stage	
clean:
	rm -rf build

