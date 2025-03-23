#ifndef SYSCALL_HEADER
#define SYSCALL_HEADER

#include <cpu/cpu.h>
#include <stdint.h>
#include <fs/fat.h>


#define __SYSCALL__ extern

enum SYSCALL_ {
    READ = 0x1,
    WRITE = 0x2,
    OPEN = 3,
    CLOSE = 4,
    GETSIZE = 5,
    FNAME = 6,
    GETWS = 7,
    GETHS = 8,
    PUT_PIXEL = 9,
    GET_PIXEL = 10,
    GETFONT_BITMAP = 11,
    EXEC = 12,
    MMAP = 13,
    MALLOC = 14,
    FREE = 15,
    GETKEY = 16,
    GETKEYSTATUS = 17,
    COPY_PIXEL = 18,
    GETVIDEOBUFFERADDRESS = 19,
    OPEN_DIR = 20,
    CLOSE_DIR = 21,
    READ_DIR = 22,
    MKDIR = 23,
    MKFILE = 24
};

uint32_t syscall_handler();

#ifdef __SYSCALL__FUNCTION__
DWORD   sys_open(const char* __filename, const char* __flags);
VOID    sys_close(DWORD __fd);
DWORD   sys_getsize(DWORD __fd);
VOID    sys_read(DWORD __fd, BYTE* __buffer, size_t __size);
VOID    sys_write(DWORD __fd, BYTE* __buffer, size_t __size);
DWORD   sys_mkdir(const char* __dirname);
DWORD   sys_mkfile(const char* __filename, char* buffer, size_t size);
DWORD   sys_opendir(const char* __dirname);
VOID    sys_closedir(DWORD __fd);
DWORD   sys_readdir(DWORD __fd, FILE* __output);
DWORD   sys_malloc(DWORD __size);
VOID    sys_free(DWORD __ptr);
VOID    sys_mmap(DWORD __virtual_address, DWORD __size, BYTE __flags);
WORD    sys_getkeypress();
WORD    sys_getkeystatus();
#endif

__SYSCALL__ VOID        read(uint32_t __fd, uint8_t* __buffer, size_t __size);
__SYSCALL__ DWORD       open(const char* filename, const char* flags);
__SYSCALL__ VOID        close(uint32_t __fd);
__SYSCALL__ DWORD       getsize(uint32_t __fd);
__SYSCALL__ VOID        video_putpixel(int _x, int _y, uint32_t _color);
__SYSCALL__ DWORD       video_getpixel(int _x, int _y);
__SYSCALL__ VOID        video_copypixel(uint32_t _from, uint32_t _to);
__SYSCALL__ DWORD       video_getframebuffer();
__SYSCALL__ WORD        video_getwidthscreen();
__SYSCALL__ WORD        video_getheightscreen();
__SYSCALL__ DWORD       font_getcharbitmap(char _c);
__SYSCALL__ DWORD       _malloc(uint32_t __size);
__SYSCALL__ VOID        _free(void* __ptr);
__SYSCALL__ VOID        mmap(uint32_t __virtual_address, size_t __size, uint8_t __flags);
__SYSCALL__ WORD        getkeypress();
__SYSCALL__ WORD        getkeystatus();
__SYSCALL__ DWORD       opendir(const char* folder_name);
__SYSCALL__ DWORD       closedir(uint32_t __dir);
__SYSCALL__ DWORD       readdir(uint32_t __dir, FILE* __output);
__SYSCALL__ DWORD       mkdir(const char* folder_name);
__SYSCALL__ DWORD       mkfile(const char* filename, char* buffer, size_t size);
#endif