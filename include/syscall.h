#ifndef SYSCALL_HEADER
#define SYSCALL_HEADER

#include <cpu/cpu.h>
#include <stdint.h>

#define __SYSCALL__ extern

enum SYSCALL_ {
    READ = 0x1,
    WRITE,
    OPEN,
    CLOSE,
    GETSIZE,
    FNAME,
    GETWS,
    GETHS,
    PUT_PIXEL,
    GET_PIXEL,
    GETFONT_BITMAP,
    EXEC
};

uint32_t syscall_handler();

#ifdef __SYSCALL__FUNCTION__
DWORD   sys_open(const char* __filename, const char* __flags);
VOID    sys_close(DWORD __fd);
DWORD   sys_getsize(DWORD __fd);
VOID    sys_read(DWORD __fd, BYTE* __buffer, size_t __size);
VOID    sys_write(DWORD __fd, BYTE* __buffer, size_t __size);
DWORD   sys_mkdir(const char* __dirname);
DWORD   sys_mkfile(const char* __filename);
#endif

__SYSCALL__ VOID        read(uint32_t __fd, uint8_t* __buffer, size_t __size);
__SYSCALL__ DWORD       open(const char* filename, const char* flags);
__SYSCALL__ VOID        close(uint32_t __fd);
__SYSCALL__ DWORD       getsize(uint32_t __fd);
__SYSCALL__ VOID        video_putpixel(int _x, int _y, uint32_t _color);
__SYSCALL__ DWORD       video_getpixel(int _x, int _y);
__SYSCALL__ WORD        video_getwidthscreen();
__SYSCALL__ WORD        video_getheightscreen();

#endif