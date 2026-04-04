#pragma once

#define CHECK_ERROR( err ) if( EFI_ERROR( err ) ) { \
                                Print(L"Error Code on %a at %d: 0x%lx", __FILE__, __LINE__, err ); \
                                while( 1 ) \
                                    __asm__ volatile("hlt"); \
                            }

void UEFI_ShowMemoryMap( EFI_HANDLE imageHandle );
extern void go_jump( UINT64 pml4, UINT64 entry );