GCC_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-gcc
LD_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-ld
AS_COMPILER=${HOME}/opt/cross/bin/x86_64-elf-as
AR_LINKING=${HOME}/opt/cross/bin/x86_64-elf-ar

GCC_FLAGS=-m64 -ffreestanding -Wall -Wextra -nostdlib -fno-builtin -fno-stack-protector

BUILD_DIR=build_shared

C_FILE_SOURCES=$(shell find ./sources -name *.c )
AS_FILE_SOURCES=$(shell find ./sources -name *.S )

C_OBJECTS_SOURCES=$(patsubst ./sources/%.c, ${BUILD_DIR}/%.c.o, ${C_FILE_SOURCES})
AS_OBJECTS_SOURCES=$(patsubst ./sources/%.S, ${BUILD_DIR}/%.gas.o, ${AS_FILE_SOURCES})

all: show build

show:
	echo ${AS_FILE_SOURCES}
	echo ${C_FILE_SOURCES}
	echo ${C_OBJECTS_SOURCES}
	echo ${AS_OBJECTS_SOURCES}

build: make_dir ${C_OBJECTS_SOURCES} ${AS_OBJECTS_SOURCES}
	ar rcs libc.a ${AS_OBJECTS_SOURCES} ${C_OBJECTS_SOURCES}

${BUILD_DIR}/%.c.o: ./sources/%.c
	${GCC_COMPILER} ${GCC_FLAGS} -I"../kernel" -o $@ -c $<

${BUILD_DIR}/%.gas.o: ./sources/%.S
	${AS_COMPILER} --64 -o $@ $<


make_dir: clean
	mkdir ${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR}
	rm -rf libc.so
	rm -rf libc.a
