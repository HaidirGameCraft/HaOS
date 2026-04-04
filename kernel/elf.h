#pragma once

#include <type.h>
#include <fs/fat.h>

#define ISA_ARCH_NONE       0x00
#define ISA_ARCH_SPARC      0x02
#define ISA_ARCH_X86        0x03
#define ISA_ARCH_MIPS       0x08
#define ISA_ARCH_POWERPC    0x14
#define ISA_ARCH_ARM        0x28
#define ISA_SUPERH          0x2A

typedef struct {
    char  magic_number[4];  // Containing 0x7F following by 'ELF'
    byte  machine;          // Type of machine the this ELF should run
    byte  endian;
    byte  header_version;
    byte  system;
    qword unused;
    word  type;
    word  instruction_set;
    dword version;
    qword prog_entry_off;       // The Entry of program start
    qword prog_header_table_off;    // the offset of The Program Table from beginning of file
    qword sec_header_table_off;     // the offset of the Section Table from beginning of file
    dword flags;
    word  header_size;
    word  prog_header_size;
    word  count_prog;
    word  sec_header_size;
    word  count_sec;
    word  sec_index;
} __packed elf64_header_t;

#define ELF_PROG_FLAGS_X  0b0001
#define ELF_PROG_FLAGS_W  0b0010
#define ELF_PROG_FLAGS_R  0b0100

#define EPT_NULL    0x00000000
#define EPT_LOAD    0x00000001
#define EPT_DYNC    0x00000002
#define EPT_INTERP  0x00000003
#define EPT_NOTE    0x00000004
#define EPT_SHLIB   0x00000005
#define EPT_PHDR    0x00000006
#define EPT_TLS     0x00000007
#define EPT_LOOS    0x60000000
#define EPT_HIOS    0x6FFFFFFF
#define EPT_LOPRC   0x70000000
#define EPT_HIPRC   0x7FFFFFFF

typedef struct {
    dword   type_segment;

    /*
    Flags of Program:
    0x01 - Executable
    0x02 - Writable
    0x04 - Readable
    */
    dword   flags;
    qword   prog_off;       // Buffer/Data/Code offset from beginning file
    qword   virt_addr;
    qword   phys_addr;
    qword   filesize;       // Size of Segment
    qword   memsize;        // Memory size in memory ( Should be served in memory )
    qword   align;          
} __packed elf64_program_t;

typedef struct {
    dword   name;
    dword   type;
    qword   flags;
    qword   addr;
    qword   offset;
    qword   size;
    dword   link;
    dword   info;
    qword   addr_align;
    qword   entry_size;
} __attribute__((packed)) elf64_section_t;


elf64_header_t* elf64_create( const char* filename );
byte            elf64_verity( elf64_header_t* hdr );
void            elf64_showDetails( elf64_header_t* hdr );
qword           elf64_load( const char* filename );
fat_file_ptr*   elf64_loadLibrary( const char* filename );
qword           elf64_unloadLibrary( fat_file_ptr* fptr );
