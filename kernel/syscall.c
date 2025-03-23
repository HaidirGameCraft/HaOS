#define __SYSCALL__FUNCTION__

#include <cpu/cpu.h>
#include <syscall.h>
#include <memory.h>
#include <driver/disk_driver.h>
#include <driver/video_driver.h>
#include <font.h>
#include <fs/fat.h>
#include <fs/fs.h>
#include <stdio.h>
#include <string.h>

#define CREATE_BUFFER_SECTOR(buffer) char* buffer = (char*) malloc( 512 )
#define FREE_BUFFER_SECTOR(buffer) free( buffer );

uint32_t syscall_handler()
{
    uint32_t eax = 0, ecx = 0, edx = 0, ebx = 0, esp = 0, ebp = 0, esi = 0, edi = 0;
    asm volatile("pusha");
    asm volatile("movl %%eax, %0" : "=r"(eax));
    asm volatile("movl %%ecx, %0" : "=r"(ecx));
    asm volatile("movl %%edx, %0" : "=r"(edx));
    asm volatile("movl %%ebx, %0" : "=r"(ebx));
    asm volatile("movl %%esp, %0" : "=r"(esp));
    asm volatile("movl %%ebp, %0" : "=r"(ebp));
    asm volatile("movl %%esi, %0" : "=r"(esi));
    asm volatile("movl %%edi, %0" : "=r"(edi));

    struct FAT_Header* header = get_fat_header();

    switch (eax)
    {
    case READ:  // read
        {
            sys_read((uint32_t) esi, (uint8_t*) ebx, (size_t) ecx);
            eax = 0;
        }
        break;
    case WRITE:  // write
        if( esi == STDIN )
        {
            
        }
        break;
    case OPEN:  // open
    {
        eax = sys_open((char*) esi, (char*) ebx);
    }
        break;
    case CLOSE:  // close
        sys_close((uint32_t) esi);
        break;
    case GETSIZE:
        eax = sys_getsize((uint32_t) esi);
        break;
    case FNAME:  // fname ( Get Filename )
    {
        FILEADDR* fd = (FILEADDR*) esi;
        struct FAT_Header* header = get_fat_header();

        uint32_t cluster = header->cluster;
        uint32_t fat_region = header->reserved_sectors;
        uint32_t data_region =  fat_region + ( header->fat_numbers * header->sectors_per_fat );
    }
        break;
    case PUT_PIXEL:
        {
            VDriver_PutPixel(ebx, ecx, edx);
            eax = 0;
        }
        break;
    case GET_PIXEL:
        {
            eax = VDriver_GetPixel(ebx, ecx);
        }
        break;
    case GETFONT_BITMAP:
        eax = (addr_t) Font_GetBitmap((char) ebx);
        break;
    case GETWS:
        eax = VDriver_GetWidth();
        break;
    case GETHS:
        eax = VDriver_GetHeight();
        break;
    case COPY_PIXEL:
        VDriver_CopyPixel(ebx, ecx);
        break;
    case GETVIDEOBUFFERADDRESS:
        eax = (addr_t) VDriver_GetAddress();
        break;
    case MMAP: sys_mmap(edi, ecx, ebx); break;
    case MALLOC: eax = sys_malloc(ecx); break;
    case FREE: sys_free(edi); break;
    case GETKEY: eax = sys_getkeypress(); break;
    case GETKEYSTATUS: eax = sys_getkeystatus(); break;
    case OPEN_DIR: eax = sys_opendir((char*) edi); break;
    case CLOSE_DIR: sys_closedir(esi); break;
    case READ_DIR: eax = sys_readdir(esi, (FILE*) edi); break;
    case MKDIR: eax = sys_mkdir((char*) edi); break;
    case MKFILE: eax = sys_mkfile((char*) edi, (char*) esi, ecx); break;
    default:
        break;
    }

    asm volatile("popa");
    asm volatile("movl %0, %%eax" :: "r"(eax));
    return eax;
}