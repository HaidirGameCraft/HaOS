target remote :1234
watch *((unsigned long long) 0x880000) == 0xDEAD1234
continue
set $base = $rip
add-symbol-file build/UEFI/uefi.so -o $base
si
layout asm