[bits 16]
section .text
global vesa_init
vesa_init:
    ; Get the Vesa Information
    mov ax, 0x4F00
    mov di, vesaInfo
    int 0x10

    ; Get The Vesa Mode Information
    mov ax, 0x4F01
    mov cx, 0x0115
    mov di, vesaModeInfo
    int 0x10

    ; Set the Vesa Video Mode
    mov ax, 0x4F02
    mov bx, cx
    or bx, 0x4000
    int 0x10
    ret

section .data
struc VesaBlock_Info
    .signature: resb 4
    .version: resw 1
    .string_ptr: resd 1
    .capablities: resb 4
    .videoMode_ptr: resd 1
    .total_memory: resw 1
    .reserved: resb 492
endstruc

struc VesaVideoMode_Info
    .attributes: resw 1
    .firstWindow: resb 1
    .secondWindow: resb 1
    .granularity: resw 1
    .windowSize: resw 1
    .firstSegment: resw 1
    .secondSegment: resw 1
    .window_ptr: resd 1
    .pitch: resw 1
    .width: resw 1
    .height: resw 1
    .widthChar: resb 1
    .heightChar: resb 1
    .planes: resb 1
    .bpp: resb 1
    .banks: resb 1
    .memoryModel: resb 1
    .bankSize: resb 1
    .imagePages: resb 1
    .reserved: resb 1

    .redMask: resb 1
    .redPosition: resb 1
    .greenMask: resb 1
    .greenPosition: resb 1
    .blueMask: resb 1
    .bluePosition: resb 1
    .alphaMask: resb 1
    .alphaPosition: resb 1
    .directColor: resb 1

    .framebuffer: resd 1
    .offScreen_memOff: resd 1
    .offScreen_memSize: resw 1
    .zeroes: resb 206
endstruc


global vesaInfo
vesaInfo: istruc VesaBlock_Info 
    times 512 db 0
iend

align 4
global vesaModeInfo 
vesaModeInfo: istruc VesaVideoMode_Info
    times 256 db 0
iend