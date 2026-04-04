#pragma once

#include <efi/efi.h>


UINT64 Kernel_createNewPage( EFI_HANDLE imageHandle );
void Kernel_MapPage( UINT64 virtAddress, UINT64 size, UINT32 flags );
void Kernel_MapPagePhysical( UINT64 virtAddress, UINT64 physicalAddress, UINT64 size, UINT32 flags );
void Kernel_ShowPaging();
UINT64 Kernel_Alloc4KPage();
