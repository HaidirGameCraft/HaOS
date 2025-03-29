#define BOOT_STAGE_DEFINE
#include <elf/elf.h>
#include <fs/fat.h>
#include <stdio.h>
#include <string.h>

#define PML4T_INDEX(x) x >> 39 & 0x1FF
#define PDP_INDEX(x) x >> 30 & 0x1FF
#define PD_INDEX(x) x >> 21 & 0x1FF
#define PT_INDEX(x) x >> 12 & 0x1FF
extern void Halt();

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
    uint64_t* pml4t = (uint64_t*) 0x400000;
    uint64_t* pdp = (uint64_t*) 0x401000;

    uint64_t* kernel_pd = (uint64_t*) 0x406000;
    uint64_t* new_pt = (uint64_t*) 0x407000;

    for(int i = 0; i < 512; i++)
    {
        uint64_t v = ((uint64_t) 0xC0000000 + i * 0x1000);
        new_pt[i] = v | 0b11;
    }

    kernel_pd[PD_INDEX(0xC0000000)] = ((uint64_t) new_pt) | 0b11;

    if( ELF_checkFile( header ) != 0 )
    {
        return;
    }

    ELF32_Program_Header* program_table = (ELF32_Program_Header*)( buffer + header->program_offset );
    ELF32_Section_Header* section_table = (ELF32_Section_Header*)( buffer + header->section_offset );

    for(int i = 0; i < header->program_number - 1; i++) {
        ELF32_Program_Header* program = &program_table[i];

        uint64_t pt_idx = (uint64_t) PT_INDEX((uint64_t) program->virt_address);
        uint64_t pd_idx = (uint64_t) PD_INDEX((uint64_t) program->virt_address);
        uint64_t pdp_idx = (uint64_t) PDP_INDEX((uint64_t) program->virt_address);

        printh(program->virt_address, 4);
        printf(" ");
        printh(pdp_idx, 4);
        printf("\n");

        if( pdp[pdp_idx] & 0x01 )
        {
            uint64_t* pd_table = (uint64_t*)((uint64_t) pdp[pdp_idx] & ~0x1FF);
            
            if( pd_table[pd_idx] & 0x01 )
            {
                uint64_t* pt_table = (uint64_t*)(pd_table[pd_idx] & ~0x1FF);
                pt_table[pt_idx] = program->physc_address | 0x03;

                memcpy((void*) program->virt_address, buffer + program->p_offset, program->p_filesize);
            }

        }
        //memcpy((void*) program->virt_address, buffer + program->p_offset, program->p_filesize);   
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

