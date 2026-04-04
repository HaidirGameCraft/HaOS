#include <efi/efi.h>
#include <efi/efiapi.h>
#include <efi/efidef.h>
#include <efi/efierr.h>
#include <efi/efilib.h>
#include <header.h>
#include "main.h"
#include "page.h"

EFI_FILE_HANDLE GetRoot( EFI_HANDLE handle ) {
    EFI_STATUS status;
    
    // Get the Image Loaded
    EFI_LOADED_IMAGE *image_loaded = 0;
    // uefi_call_wrapper(BS->HandleProtocol, 3, handle, &imageload_guid, &image_loaded);
    Print(L"Handle Address: 0x%lx\n", handle );
    Print(L"Handle Protocol Method Address: 0x%lx\n", BS->HandleProtocol );
    status = uefi_call_wrapper( BS->HandleProtocol, 0, 
                                handle, 
                                &gEfiLoadedImageProtocolGuid, 
                                (void**) &image_loaded );

    if( EFI_ERROR( status ) )
    {
        Print(L" Call for Image Loaded failed: 0x%lx\n", status );
        while( 1 )
            __asm__ volatile("hlt");
    }


    // Get the FileSystem based on What Filesystem of Image Loaded are load
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *filesystem = NULL;
    status = uefi_call_wrapper( BS->HandleProtocol, 0, 
                                image_loaded->DeviceHandle, 
                                &gEfiSimpleFileSystemProtocolGuid,
                                &filesystem);

    if( EFI_ERROR( status ) ) 
    {
        Print(L"Call for calling Filesystem: 0x%lx\n", status );
        while( 1 )
            __asm__ volatile("hlt");
    }

    // Opening Volume
    EFI_FILE_HANDLE rootDir;
    status = uefi_call_wrapper( filesystem->OpenVolume, 0, filesystem, &rootDir );

    if( EFI_ERROR( status ) )
    {
        Print(L"Call for getting RootDir/Volume of Filesystem Error: 0x%lx\n", status );
        while( 1 ) __asm__ volatile("hlt");
    }

    return rootDir;
}

EFI_FILE_HANDLE OpenFile( EFI_FILE_HANDLE Root, const CHAR16* filename ) {
    EFI_FILE_HANDLE file;
    EFI_STATUS status;
    status = uefi_call_wrapper( Root->Open, 0, Root, &file, filename, EFI_FILE_MODE_READ, 0 );

    if( EFI_ERROR( status ) )
    {
        Print(L"%S is Not Found (Err: 0x%lx)\n", filename, status );
        while( 1 ) __asm__ volatile("hlt");
    }

    return file;
}

kernel_header_info_t* LoadKernel( EFI_HANDLE handle, const CHAR16* filename ) {
    EFI_FILE_HANDLE Root = GetRoot( handle );
    Print(L"Finding Root is Done\n");
    EFI_FILE_HANDLE file = OpenFile( Root, filename );
    Print(L"Finding File is Done\n");

    // Get Information from File
    EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
    UINT64 sizeFileInfo = 512;
    EFI_FILE_INFO *fileInfo = NULL;
    EFI_STATUS status = 0;
    status = uefi_call_wrapper( BS->AllocatePool, 0, EfiBootServicesData, sizeFileInfo, &fileInfo );
    CHECK_ERROR( status )
    
    status = uefi_call_wrapper( file->GetInfo, 0, file, &gEfiFileInfoGuid, &sizeFileInfo, fileInfo );
    CHECK_ERROR( status );

    Print(L"FileSize: 0x%x\n", fileInfo->FileSize );
    Print(L"Filename: %s\n", fileInfo->FileName );

    // Now load the Program and mapping
    Kernel_createNewPage( handle );

    // Get Kernel Header
    EFI_VIRTUAL_ADDRESS kernel_virt_address = 0xFFFFFFFF80100000;
    EFI_PHYSICAL_ADDRESS kernel_physc_address = 0x0000000000100000;
    UINT64 fileSize = fileInfo->FileSize + 256;
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAddress, EfiLoaderData, (UINT64)( fileSize / EFI_PAGE_SIZE ) + ( fileSize % EFI_PAGE_SIZE > 0 ) , &kernel_physc_address ) )
    CHECK_ERROR( uefi_call_wrapper( file->Read, 0, file, &fileSize, (void*) kernel_physc_address ) )

    kernel_header_info_t* kernelInfo = ( kernel_header_info_t* ) kernel_physc_address;
    UINT64 kernelSize = kernelInfo->address_end - kernelInfo->address_start;
    Kernel_MapPagePhysical( kernel_virt_address, kernel_physc_address, kernelSize, (1 << 0) | ( 1 << 1 ) );

    Print(L"Kernel Identification: %a\n", kernelInfo->identifier );
    Print(L"Kernel ImageStart: 0x%lx\n", kernelInfo->address_start );
    Print(L"Kernel ImageEnd: 0x%lx\n", kernelInfo->address_end );
    Print(L"Kernel Entry: 0x%lx\n", kernelInfo->entry_start );

    status = uefi_call_wrapper( BS->FreePool, 0, fileInfo );
    CHECK_ERROR( status )

    // Now get gkrnl
    EFI_FILE_HANDLE gkrnlfile = OpenFile( Root, L"gkrnl.bin" );
    UINT64 gkrnlfileSize = 512;
    CHECK_ERROR( uefi_call_wrapper( gkrnlfile->Read, 0, gkrnlfile, &gkrnlfileSize, (void*) 0x15000 ) );
    Kernel_MapPagePhysical(0x15000, 0x15000, 0x1000, ( 1 << 1 ) | ( 1 << 0) );

    // After Than that we need to load font
    EFI_FILE_HANDLE fontFile = OpenFile( Root, L"font.map" );
    UINT64 fontFileSize = 0x2000;
    UINT64 fontData = 0;
    CHECK_ERROR( uefi_call_wrapper( BS->AllocatePages, 0, AllocateAnyPages, EfiLoaderData, 2, &fontData ) );
    CHECK_ERROR( uefi_call_wrapper( fontFile->Read, 0, fontFile, &fontFileSize, (void*) fontData ));
    kernelInfo->font_data = fontData;
    Kernel_MapPagePhysical( fontData, fontData, 0x2000, ( 1 << 0 ) | ( 1 << 1 ) );

    return kernelInfo;
}



EFI_STATUS EFIAPI efi_main( EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *sys_table ) {
    InitializeLib(image_handle, sys_table);
    Print(L"Hello, World from UEFI\n");
    
    UEFI_ShowMemoryMap( image_handle );
    kernel_header_info_t* infoKernel = LoadKernel( image_handle, L"kernel.bin" );

    // Get Graphics Address
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicOutput = NULL;
    CHECK_ERROR( uefi_call_wrapper( BS->LocateProtocol, 0, &gEfiGraphicsOutputProtocolGuid, NULL, &graphicOutput ) );

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* infoMode = graphicOutput->Mode->Info;

    Print(L"\nGraphic Mode Information: ---\n");
    Print(L"Version: 0x%lx\n", infoMode->Version);
    Print(L"Horizontal Resolution: %d\n", infoMode->HorizontalResolution);
    Print(L"Vertical Resolution: %d\n", infoMode->VerticalResolution);
    Print(L"Pixel Format: %d\n", infoMode->PixelFormat);

    Print(L"Framebuffer Address: 0x%lx\n", graphicOutput->Mode->FrameBufferBase );
    Print(L"Size of FrameBuffer: %d\n", graphicOutput->Mode->FrameBufferSize );

    infoKernel->width = infoMode->HorizontalResolution;
    infoKernel->height = infoMode->VerticalResolution;
    infoKernel->bytes_per_pixel = 32;
    infoKernel->framebuffer = graphicOutput->Mode->FrameBufferBase;
    infoKernel->size_of_framebuffer = graphicOutput->Mode->FrameBufferSize;

    Kernel_ShowPaging();

    // Exit the BootService
    UINT64 mapKey = 0;
    UINT64 descriptorSize = 0;
    UINT32 descriptorVersion = 0;
    UINT64 number_of_entries = 0;
    EFI_MEMORY_DESCRIPTOR* mem_desc = LibMemoryMap( &number_of_entries, &mapKey, &descriptorSize, &descriptorVersion );

    EFI_PHYSICAL_ADDRESS entry = infoKernel->entry_start;

    CHECK_ERROR( uefi_call_wrapper(BS->ExitBootServices, 0, image_handle, mapKey ) );


    ((UINT64*) 0x880000)[0] = 0xDEAD1234;
    go_jump( 0x1000, entry );

    while( 1 )
        __asm__ volatile("hlt");    // Stuck the CPU

    return EFI_SUCCESS;
}


void UEFI_ShowMemoryMap( EFI_HANDLE imageHandle ) {
    

    UINT64 mapKey = 0;
    UINT64 descriptorSize = 0;
    UINT32 descriptorVersion = 0;
    UINT64 number_of_entries = 0;
    EFI_MEMORY_DESCRIPTOR* mem_desc = LibMemoryMap( &number_of_entries, &mapKey, &descriptorSize, &descriptorVersion );
    

    Print(L"DescriptionSize=%d, DescriptionVersion=%x\n", descriptorSize, descriptorVersion );
    for( int i = 0; i < number_of_entries; i++ )
    {
        EFI_MEMORY_DESCRIPTOR* memD = (EFI_MEMORY_DESCRIPTOR*)( (UINT8*) mem_desc + i * descriptorSize );
        if( memD->Type != EfiConventionalMemory )
            continue;

        Print(L"%d: Type=%x, Pad=%x, Physical=0x%lx, Virtual=0x%lx, Pages=0x%lx, Attribute=0x%lx\n", i, memD->Type, memD->Pad, memD->PhysicalStart, memD->VirtualStart, memD->NumberOfPages, memD->Attribute );
    }

    FreePool( mem_desc );
}