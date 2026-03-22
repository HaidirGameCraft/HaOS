#pragma once

#include <type.h>

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

typedef struct {
    dword   type_segment;
    dword   flags;
    qword   prog_off;       // Buffer/Data/Code offset from beginning file
    qword   virt_addr;
    qword   phys_addr;
    qword   filesize;       // Size of Segment
    qword   memsize;        // Memory size in memory ( Should be served in memory )
    qword   align;          
} __packed elf64_program_t;


elf64_header_t* elf64_create( const char* filename );
byte            elf64_verity( elf64_header_t* hdr );
void            elf64_showDetails( elf64_header_t* hdr );
qword           elf64_load( const char* filename );
