#include "alloc.h"
#include "page.h"
#include "config.h"

typedef struct __alloc_block_t {
    qword size;
    byte available;
    struct __alloc_block_t* next;
} __attribute__((packed)) alloc_block_t;

alloc_block_t* heap_start = NULL;
alloc_block_t* heap_end = NULL;
qword heap_size = 0;

void  init_alloc() {
    heap_start = ( alloc_block_t* ) __heap_start;
    page_mapv( (qword) heap_start, 0x1000 );        // Mapped the heap address intio page table

    heap_start->available = 1;
    heap_start->size = 0x1000;
    heap_start->next = NULL;
    heap_size += heap_start->size;

    heap_end = ( alloc_block_t* ) heap_start;
}

void* new_alloc( qword size ) {
    if( size == 0 ) return NULL;

    alloc_block_t* current = heap_start;
    while( current != NULL )
    {
        // Check if the current is available
        if( current->available == 1 )
        {
            // Check if the current size is equal to required size with the size of alloc_block_t
            if( current->size == size + sizeof( alloc_block_t ) )
            {
                current->available = 0;
                return (void*)( (qword) current + sizeof( alloc_block_t ) ); // returning the available block without any change
            } else if ( current->size > size + sizeof( alloc_block_t ))
            {
                current->available = 0;
                dword size_block = size + sizeof( alloc_block_t );
                
                dword remain_size = current->size - ( size_block );
                if( remain_size < sizeof( alloc_block_t ) )
                    return (void*)( (qword) current + sizeof( alloc_block_t ) );
                
                // Split the block ( if remains ) with new block
                alloc_block_t* new_block = (alloc_block_t*)((qword) current + size_block );

                // Checking there is out of boundary or not
                qword end_heap = (qword) heap_start + heap_size;
                if( end_heap < ( qword ) new_block + size_block )
                {
                    // Need alloc new frame to extend alloc
                    page_mapv( ((qword) heap_start + heap_size), 0x1000 );
                    heap_size += 0x1000;

                    remain_size = ((qword) heap_start + heap_size ) - ( qword ) new_block; 
                }

                new_block->size = remain_size;
                new_block->next = current->next;
                new_block->available = 1;
                current->next = new_block;
                current->size = size_block;

                // Check if new block is end
                if( new_block->next == NULL )
                    heap_end = new_block;

                return (void*)((qword) current + sizeof( alloc_block_t ) );
            }
        }

        // and check if the current is end and go to next block
        if( current->next == NULL )
            heap_end = current;
        current = current->next;
    }

    // If there is no block that need to give, make new page
    page_mapv( (qword) heap_start + heap_size, 0x1000 );
    heap_size += 0x1000;
    return new_alloc( size );
}

void  free_alloc( void* ptr ) {
    if( ptr == NULL ) return;

    alloc_block_t* block = (alloc_block_t*)((qword) ptr - sizeof( alloc_block_t ) );
    alloc_block_t* prev_block = block; // find the previous block;
    alloc_block_t* next_block = block->next;

    if( (qword)(prev_block) == (qword)(heap_start) )
    {
        block->available = 1;
        if( next_block != NULL && next_block->available == 1 )
        {
            block->size += next_block->size;
            block->next = next_block->next;
        }
    } else {
        while( prev_block->next != NULL )
        {
            if( prev_block->next == block )
                break;

            prev_block = prev_block->next;
            if( prev_block == NULL )
                return;
        }

        block->available = 1;
        if( next_block != NULL && next_block->available == 1 )
        {
            block->size += next_block->size;
            block->next = next_block->next;
        }

        if( prev_block->available == 1 )
        {
            prev_block->size += block->size;
            prev_block->next = block->next;
        }
    }
}