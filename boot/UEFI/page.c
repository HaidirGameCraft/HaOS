#include "page.h"
#include "main.h"
#include <efi/efilib.h>

UINT64* pml4_table = NULL;
UINT8* bitmap = NULL;
UINT64 Kernel_createNewPage( EFI_HANDLE imageHandle ) {
    // Get the image loaded
    EFI_LOADED_IMAGE *loadedImage = NULL;
    EFI_STATUS status = uefi_call_wrapper( BS->HandleProtocol, 0, imageHandle, &gEfiLoadedImageProtocolGuid, &loadedImage );
    CHECK_ERROR( status );

    Print(L"Image Loaded Information: ---\n");
    Print(L"Image Base: 0x%lx\n", loadedImage->ImageBase );
    Print(L"Image Size: 0x%lx (%i)\n", loadedImage->ImageSize, loadedImage->ImageSize);


    pml4_table = (UINT64*) 0x1000;
    EFI_PHYSICAL_ADDRESS pdpt_table = 0x2000;
    EFI_PHYSICAL_ADDRESS pdt_table = 0x3000;
    EFI_PHYSICAL_ADDRESS pt_table = 0x4000;
    EFI_PHYSICAL_ADDRESS frame_table = 0x5000;
    bitmap = (UINT8*) 0x6000;

    
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &pml4_table ) )
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &pdpt_table ) )
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &pdt_table ) )
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &pt_table ) )
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &frame_table ) )
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &bitmap ) )
    
    Print(L"PMLT4 Table Address: 0x%lx\n", pml4_table );
    Print(L"PDPT Table Address: 0x%lx\n", pdpt_table );
    Print(L"PDT Table Address: 0x%lx\n", pdt_table );
    Print(L"PT Table Address: 0x%lx\n", pt_table );
    Print(L"FRAME Table Address: 0x%lx\n", frame_table );
    
    // Free
    CHECK_ERROR( uefi_call_wrapper(BS->SetMem, 0, pml4_table, EFI_PAGE_SIZE, 0 ) )
    CHECK_ERROR( uefi_call_wrapper(BS->SetMem, 0, pdpt_table, EFI_PAGE_SIZE, 0 ) )
    CHECK_ERROR( uefi_call_wrapper(BS->SetMem, 0, pdt_table, EFI_PAGE_SIZE, 0 ) )
    CHECK_ERROR( uefi_call_wrapper(BS->SetMem, 0, pt_table, EFI_PAGE_SIZE, 0 ) )
    CHECK_ERROR( uefi_call_wrapper(BS->SetMem, 0, frame_table, EFI_PAGE_SIZE, 0 ) )
    CHECK_ERROR( uefi_call_wrapper(BS->SetMem, 0, bitmap, EFI_PAGE_SIZE, 0 ) );


    ((UINT64*) pml4_table)[  0] = ((UINT64) pdpt_table) | ( 1 << 0 ) | ( 1 << 1 );
    ((UINT64*) pdpt_table)[  0] = ((UINT64) pdt_table) | ( 1 << 0 ) | ( 1 << 1 );
    ((UINT64*) pdt_table)[  0] = ((UINT64) pt_table) | ( 1 << 1 ) | ( 1 << 0 );

    // Mapped PML4, PDPT, PDT, PT and FRAME to current address
    for( int i = 0; i < 7; i++ )
    {
        ((UINT64*) pt_table)[i] = ( i * EFI_PAGE_SIZE ) | ( 1 << 1 ) | ( 1 << 0 );
        ((UINT8*) bitmap)[(UINT64)(i / 8)] |= 1 << ((UINT64)(i % 8));
    }

    ((UINT64*) pdt_table)[511] = ((UINT64) frame_table) | ( 1 << 1 ) | ( 1 << 0 );
}

UINT64 Kernel_Alloc4KPage() {
    for( int i = 0; i < EFI_PAGE_SIZE; i++ )
    {
        UINT8 __b = ((UINT8*) 0x6000)[i];
        if( __b == 0xFF )
            continue;

        for( int j = 0; j < 8; j++ )
        {
            // It served
            if( !(__b >> j & 1) )
            {
                EFI_PHYSICAL_ADDRESS phys = ( i * 8 + j ) * EFI_PAGE_SIZE;
                CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, 1, &phys ) )
                CHECK_ERROR( uefi_call_wrapper( BS->SetMem, 0, phys, EFI_PAGE_SIZE, 0 ) )
                bitmap[i] |= 1 << j;
                return phys;
            }
        }
    }

    return -1;
}
#define CHECK_AND_CREATE_FRAME(x, idx, table)   if( !((x) & 1) ) \
                                                    (x) = Kernel_Alloc4KPage() | flags; \
                                                UINT64* table = (UINT64*)( (x) & ~0xFFF );

void Kernel_MapPagePhysical( UINT64 virtAddress, UINT64 physicalAddress, UINT64 size, UINT32 flags ) {
    UINT64 length_pages = (UINT64)( size / EFI_PAGE_SIZE ) + ( size % EFI_PAGE_SIZE > 0 );
    for( UINT64 i = 0; i < length_pages; i++ )
    {
        EFI_VIRTUAL_ADDRESS virt = virtAddress + i * EFI_PAGE_SIZE;
        EFI_PHYSICAL_ADDRESS physc = physicalAddress + i * EFI_PAGE_SIZE;

        UINT64 pml4_index = ( virt >> 39 ) & 0x1FF;
        UINT64 pdpt_index = ( virt >> 30 ) & 0x1FF;
        UINT64 pdt_index = ( virt >> 21 ) & 0x1FF;
        UINT64 pt_index = ( virt >> 12 ) & 0x1FF;

        
        CHECK_AND_CREATE_FRAME( pml4_table[pml4_index], pml4_index, pdpt_table );
        CHECK_AND_CREATE_FRAME( pdpt_table[pdpt_index], pdpt_index, pdt_table );
        CHECK_AND_CREATE_FRAME( pdt_table[pdt_index], pdt_index, pt_table );

        pt_table[ pt_index ] = ( physc ) | flags;
        UINT64 indexBitmap = (UINT64)( ( physc >> 12 ) / 8 );
        UINT64 offsetBitmap = (UINT64)( (physc >> 12 ) % 8 );
        bitmap[indexBitmap] |= 1 << offsetBitmap;
    }
}

void Kernel_ShowPaging() {
    for( int i = 0; i < 512; i++ )
    {
        UINT64 __pml4 = pml4_table[i];
        if( __pml4 == 0 ) continue;

        Print(L"PML4T[%d]: 0x%lx\n", i, __pml4 );
        UINT64* pdpt_table = (UINT64*)((UINT64) __pml4 & ~0xFFF);
        for( int j = 0; j < 512; j++ )
        {
            UINT64 __pdpt = pdpt_table[j];
            if( __pdpt == 0 )
                continue;

            Print(L"--PDPT[%d]: 0x%lx\n", j, __pdpt );
            UINT64* pdt_table = (UINT64*)( (UINT64) __pdpt & ~0xFFF );
            for( int k = 0; k < 512; k++ )
            {
                UINT64 __pdt = pdt_table[k];
                if( __pdt == 0 )
                    continue;

                Print(L"---PDT[%d]: 0x%lx\n", k, __pdt );
                UINT64* pt_table = (UINT64*)((UINT64) __pdt & ~0xFFF );
                for( int m = 0; m < 512; m++ )
                {
                    UINT64 __pt = pt_table[m];
                    if( __pt == 0 ) continue;
                    Print(L"----PT[%d]: 0x%lx\n", m, __pt );
                }
            }
        }
    }
}

void Kernel_MapPage( UINT64 virtAddress, UINT64 size, UINT32 flags ) {
    UINT64 length_pages = (UINT64)( size / EFI_PAGE_SIZE ) + ( size % EFI_PAGE_SIZE > 0 );
    for( UINT64 i = 0; i < length_pages; i++ )
    {
        EFI_VIRTUAL_ADDRESS virt = virtAddress + i * EFI_PAGE_SIZE;

        UINT64 pml4_index = ( virt >> 39 ) & 0x1FF;
        UINT64 pdpt_index = ( virt >> 30 ) & 0x1FF;
        UINT64 pdt_index = ( virt >> 21 ) & 0x1FF;
        UINT64 pt_index = ( virt >> 12 ) & 0x1FF;

        
        CHECK_AND_CREATE_FRAME( pml4_table[pml4_index], pml4_index, pdpt_table );
        CHECK_AND_CREATE_FRAME( pdpt_table[pdpt_index], pdpt_index, pdt_table );
        CHECK_AND_CREATE_FRAME( pdt_table[pdt_index], pdt_index, pt_table );

        pt_table[ pt_index ] = ( Kernel_Alloc4KPage() ) | flags;
    }
}