# HaOS
HaOS is the operating system that created from scratch. This operating system is 32 bits which is the limit of RAM is 4GB and it cannot be use on 64 bits

## Operating System
What is Operating System. Operating System is the system that operate on machine like computer or laptop to run the program. The exists Operating System, OS is Windows, MacOS, Linux and more.

## Compile and Run
```sh
# Compile
make

# Run the OS (QEMU Only)
make run
```

## Details
- This is operating system will run under bootloader (Legacy Mode, BIOS) not UEFI, due the lack of information of UEFI, how to read disk, mapped and make new page directory after running the kernel with the kernel is Protected Mode (32 bits) not Long Mode (64 bits)
- Since kernel is 1 MB above, need to mapped into higher half kernel, meaning need to make/enable paging from bootloader by providing some frame to save:
    - Page Bitmap: (Addr=0x1000, Size=0x1000)
    - Page Directory: (Addr=0x2000, Size=0x1000)
    - First Page Table: (Addr=0x3000, Size=0x1000)
    - Frame Page Table: (Addr=0x4000, Size=0x1000)
- Changing the kernel address into higher half kernel (0xC0100000), easy the program running above 1 MB
- The code is messy right now without the details of comment due to fix/change/abjust/improve the program. I will clean the code next time.


## References And Acknowledges
- [OSDev Wiki](https://wiki.osdev.org): The valuable resource of Operating System
- [pcface](https://github.com/susam/pcface/tree/main#): The font bitmap
