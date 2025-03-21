#ifndef ELF32_HEADER
#define ELF32_HEADER

#include <stdint.h>

typedef struct {
    uint8_t identifier[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t program_offset;
    uint32_t section_offset;
    uint32_t flags;
    uint16_t entry_size;
    uint16_t program_size;
    uint16_t program_number;
    uint16_t section_size;
    uint16_t section_number;
    uint16_t str_index;
} __attribute__((packed)) ELF32_Header;

enum ELF_Identifier {
    EI_MAGIC0 = 0,
    EI_MAGIC1,
    EI_MAGIC2,
    EI_MAGIC3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
    EI_OSABI,
    EI_ABIVER,
    EI_PAD
};

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    addr_t virt_address;
    addr_t physc_address;
    size_t p_filesize;
    size_t p_memsize;
    uint32_t p_flags;
    uint32_t p_align;
} C_PACKED ELF32_Program_Header;

typedef struct {
    uint32_t s_name;
    uint32_t s_type;
    uint32_t s_flags;
    addr_t s_address;
    uint32_t s_offset;
    uint32_t s_size;
    uint32_t s_link;
    uint32_t s_info;
    uint32_t s_addralign;
    uint32_t s_entry_size;
} C_PACKED ELF32_Section_Header;

ELF32_Header ELF_readHeaderFile(char* buffer);
uint8_t ELF_checkFile(ELF32_Header* header);
void ELF_readFile(ELF32_Header* header, char* buffer);

#endif