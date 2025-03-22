#define BOOT_STAGE_DEFINE
#include <elf/elf.h>
#include <fs/fat.h>
#include <stdio.h>
#include <string.h>

uint8_t reserved_buffer[512] = {0};

ELF32_Header ELF_readHeaderFile(char* buffer) {
    ELF32_Header header;
    memcpy(&header, buffer, sizeof( ELF32_Header ));
    return header;
}
uint8_t ELF_checkFile(ELF32_Header* header) {
    if( header->identifier[0] != 0x7F )
    {
        printf("ELF32 Magic Number is Not same\n");
        return 1;
    }

    if( strncmp(&header->identifier[1], "ELF", 3) != 0 )
    {
        printf("ELF32 'ELF' Is Not Define\n");
        return 2;
    }

    return 0;
}

void ELF_readFile(ELF32_Header* header, char* buffer) {
    uint32_t* page_dir = (uint32_t*) 0x00400000;

    if( ELF_checkFile( header ) != 0 )
    {
        return;
    }

    ELF32_Program_Header* program_table = (ELF32_Program_Header*)( buffer + header->program_offset );
    ELF32_Section_Header* section_table = (ELF32_Section_Header*)( buffer + header->section_offset );

    for(int i = 0; i < header->program_number; i++) {
        ELF32_Program_Header* program = &program_table[i];

        uint32_t pt_idx = (program->virt_address) >> 12 & 0x3FF;
        uint32_t pd_idx = program->virt_address >> 22;

        if( page_dir[pd_idx] & 0x01 )
        {
            uint32_t* page_table = (uint32_t*)(page_dir[pd_idx] & ~0x3FF);
            page_table[pt_idx] = program->physc_address | 0x03;

            memcpy((void*) program->virt_address, buffer + program->p_offset, program->p_filesize);
        }
    }

    // for(int i = 0; i < header->section_number; i++) {
    //     ELF32_Section_Header* section = &section_table[i];

    //     uint32_t pt_idx = (section->) >> 12 & 0x3FF;
    //     uint32_t pd_idx = section->virt_address >> 22;

    //     if( page_dir[pd_idx] & 0x01 )
    //     {
    //         uint32_t* page_table = (page_dir[pd_idx] & ~0x3FF);
    //         page_table[pt_idx] = section->physc_address | 0x03;

    //         memcpy(section->virt_address, buffer + section->p_offset, section->p_filesize);
    //     }
    // }
}

