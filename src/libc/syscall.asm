%define syscall int 0x80

section .text
global read
read:
    mov eax, 1
    mov esi, dword [esp + 4]
    mov ebx, dword [esp + 8]
    mov ecx, dword [esp + 12]
    int 0x80
    ret
global open
open:
    mov eax, 3
    mov esi, dword [esp + 4]
    mov ebx, dword [esp + 8]
    int 0x80
    ret

global close
close:
    mov eax, 4
    mov esi, dword [esp + 4]
    int 0x80
    ret

global getsize
getsize:
    mov eax, 5 
    mov esi, dword [esp + 4]
    int 0x80
    ret

global fname
fname:
    mov eax, 6
    mov esi, dword [esp + 4]
    int 0x80
    ret

global video_putpixel
video_putpixel:
    mov eax, 9
    mov ebx, dword [esp + 4]
    mov ecx, dword [esp + 8]
    mov edx, dword [esp + 12]
    int 0x80
    ret

global video_getpixel
video_getpixel:
    mov eax, 10
    mov ebx, dword [esp + 4]
    mov ecx, dword [esp + 8]
    int 0x80
    ret

global video_getwidthscreen
video_getwidthscreen:
    mov eax, 7
    int 0x80
    ret

global video_getheightscreen
video_getheightscreen:
    mov eax, 8
    int 0x80
    ret

global mmap
mmap:
    mov eax, 13
    mov edi, [esp + 4]
    mov ecx, [esp + 8]
    mov ebx, [esp + 12]
    int 0x80
    ret

global _malloc
_malloc:
    mov eax, 14
    mov ecx, [esp + 4]
    int 0x80
    ret

global _free
_free:
    mov eax, 14
    mov ecx, [esp + 4]
    int 0x80
    ret

global font_getcharbitmap
font_getcharbitmap:
    mov eax, 11
    mov ebx, dword [esp + 4]
    syscall
    ret

global getkeypress
getkeypress:
    mov eax, 16
    syscall
    ret

global getkeystatus
getkeystatus:
    mov eax, 17
    syscall
    ret

global video_copypixel
video_copypixel:
    mov eax, 18
    mov ebx, dword [esp + 4]
    mov ecx, dword [esp + 8]
    syscall
    ret
global video_getframebuffer
video_getframebuffer:
    mov eax, 19
    syscall
    ret

; opendir(const char* folder_name);
global opendir
opendir:
    mov eax, 20
    mov edi, dword [esp + 4]
    int 0x80
    ret
; closedir(uint32_t __dir);
global closedir
closedir:
    mov eax, 21
    mov esi, dword [esp + 4]
    int 0x80
    ret
; readdir(uint32_t __dir, FILE* __output);
global readdir
readdir:
    mov eax, 22
    mov esi, dword [esp + 4]
    mov edi, dword [esp + 8]
    int 0x80
    ret

; mkdir(const char* folder_name);
global mkdir
mkdir:
    mov eax, 23
    mov edi, dword [esp + 4]
    int 0x80
    ret
; mkfile(const char* filename, char* buffer, size_t size);
global mkfile
mkfile:
    mov eax, 24
    mov edi, dword [esp + 4]
    mov esi, dword [esp + 8]
    mov ecx, dword [esp + 12]
    int 0x80
    ret
    
global HALT
HALT:
    cli
    hlt
    jmp $