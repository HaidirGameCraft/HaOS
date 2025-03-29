# HaOS ( Operating System )
Hi, Welcome to HaOS. This is my learning project to know how **"How Computers Works?", "How OS works?", "Making OS is easy or not?"**.
I like challenging myself to do this complex code project. Let`s me tell you, HaOS is Operating System (UNOFFICIAL to release and get license) that starting from ***BootLoader** -> **Stage Loader** -> **Boot Stage** -> **Kernel***.

This might not be COMPLETE due the lack of time and knowledge. I can update this project to be a functional and useful for daily use like Text Editing, Internet and More. During this time, I will make basic first.

### Inspiration
This project inspire me from [OSDev](https://wiki.osdev.org/) to learning about it. This website is useful to let me know **The Complexity of OS Development**.

## New Update
1. TTY (User-space)
    - Add new command on terminal like
        - CLEAR
        - PRINT \< string... \>
        - MKDIR
        - LS
    - making vdriver clear fastest

## In Coming Feature
2. For x86_64 (64 Bits OS)
    - I`m working for x86_64 to make this OS support. Its's hard to make. So, I try to find the code from online source. Hopefully, This may be work.
    
## Old Update
1. File System (FAT32)
    - Added:
        - make_dir
        - make_entry
        - open_dir
        - read_next_dir
2. Paging
    - Add less *page_table* on boot_stage and remove the useful of all page_table on Kernel 
    (It called paging at all/full the virtual memory)
    - paging at the same address
    - Renew Page_SetMapping
3. ELF
    - Adding ELF function to read elf file from disk and replace the code to specific memory easily other than Binary file
4. TTY (User-space)
    - make user-space terminal without touching the kernel

## Syscall (Kernel -> User-Space)
at int 0x80, following this:
| eax | Name | ebx | ecx | edi | esi |
|-----|-----| ----- | ----- | ----- | ----- |
| 0x01 | read | uint8_t* buffer | size_t size |  | uint32_t __fd |
| 0x02 | None |  |  |  |  |
| 0x03 | open | uint8_t* buffer |  |  | uint32_t __fd |
| 0x04 | close |  |  |  | uint32_t __fd |
| 0x05 | getsize |  |  |  | uint32_t __fd |
| 0x06 | None |  |  |  |  |
| 0x07 | video_getwidthscreen |  | |  |  |
| 0x08 | video_getheightscreen | | | | |
| 0x09 | video_putpixel | int x | int y | uint32_t color | |
| 0x0A | video_getpixel | int x | int y |  |  |
| 0x0B | font_getcharbitmap | char _c | | | |
| 0x0C | None | | | | |
| 0x0D | mmap | uint32_t virtual_memory | size_t size | uint8_t flags | |
| 0x0E | None | | | | |
| 0x0F | None | | | | |
| 0x10 | getkeypress | | | | |
| 0x11 | getkeystatus | | | | |
| 0x12 | video_copypixel | uint32_t from| uint32_t to | | |
| 0x13 | video_getframebuffer | | | | |
| 0x14 | opendir | | | const char* filename | |
| 0x15 | closedir | | | | uint32_t __dir |
| 0x16 | readdir | | | FILE* output| uint32_t __dir |
| 0x17 | mkdir | | | const char* folder_name | |
| 0x18 | mkfile | | size_t size | const char* filename | char* buffer |

## Installing
Download or Copy this project
```sh
git clone https://github.com/HaidirGameCraft/HaOS.git
cd HaOS
```

To Compile and run via qemu only
```bash
make
make run
```

## Conclusion
I hope this project can make me more creative and improve my knowledge as students. Thank you