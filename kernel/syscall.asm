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

global HALT
HALT:
    cli
    hlt
    jmp $