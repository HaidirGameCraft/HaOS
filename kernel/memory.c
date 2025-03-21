#include <memory.h>
#include <page.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t free;
    size_t size_block;
    addr_t next;
} __attribute__((packed)) memory_block_t;

memory_block_t* heap_start = (memory_block_t*) VIRTUAL_HEAP_MEMORY_START;
size_t size_heap = VIRTUAL_HEAP_MEMORY_END - VIRTUAL_HEAP_MEMORY_START;

void memory_initialize() {
    heap_start->free = 1;
    heap_start->size_block = size_heap;
    heap_start->next = 0;

    //Page_SetMapping(VIRTUAL_HEAP_MEMORY_START, ATTR_PAGE_RW | ATTR_PAGE_PRESENT, size_heap);
}

uint32_t heap_size() {
    int r = 0;
    memory_block_t* current = heap_start;
    while( current != 0 )
    {
        if( !current->free )
        {
            r += current->size_block;
        }
        current = (memory_block_t*) current->next;
    }
    return r;
}

void* malloc(size_t size) {
    if( size <= 0 )
        return 0;

    memory_block_t* current = heap_start;
    if( current->next == 0)
    {
        memory_block_t* new_block = current + size + sizeof( memory_block_t );
        new_block->size_block = current->size_block - size - sizeof( memory_block_t );
        new_block->next = current->next;
        new_block->free = 1;
        
        current->free = 0;
        current->next = (addr_t) new_block;
        current->size_block = size + sizeof( memory_block_t );
        return (void*) current + sizeof( memory_block_t ) ;
    }

    while( current != 0 )
    {
        if( current->free && current->size_block > size + sizeof( memory_block_t ) )
        {
            current->free = 0;
            if( (current->size_block - size - sizeof( memory_block_t )) <= sizeof( memory_block_t ))
                return (void*) current + sizeof( memory_block_t );
            
            // Creating New Block to split
            memory_block_t* new_block = current + size + sizeof( memory_block_t );
            new_block->size_block = current->size_block - size - sizeof( memory_block_t );
            new_block->next = current->next;
            new_block->free = 1;

            current->free = 0;
            current->next = (addr_t) new_block;
            current->size_block = size + sizeof( memory_block_t );
            break;
        }
        current = (memory_block_t*) current->next;
    }

    if( current != 0 )
        return (void*) current + sizeof( memory_block_t );
    
    return 0;
}

void* calloc(void* ptr) {
    if( !ptr )
        return NULL;

    memory_block_t* block = (memory_block_t*)(ptr - sizeof( memory_block_t ));
    void* new_alloc = malloc( block->size_block - sizeof( memory_block_t ) );
    if( new_alloc == NULL )
        return NULL;

    memcpy(new_alloc, ptr, block->size_block - sizeof( memory_block_t ));
    return new_alloc;
}

void free(void* ptr) {
    if( !ptr || ptr == NULL)
        return;

    memory_block_t* current = (memory_block_t*)( ptr - sizeof( memory_block_t ));
    memory_block_t* next = (memory_block_t*) current->next;
    memory_block_t* prev = heap_start;
    while( prev->next != 0 )
    {
        if( prev->next == (addr_t) current )
            break;

        prev = (memory_block_t*) prev->next;
    }

    current->free = 1;
    if( next != 0 )
    {
        if( next->free == 1 )
        {
            // Merge Block
            current->size_block += next->size_block;
            current->next = next->next;
        }
    }

    if( prev != 0 )
    {
        if( prev->free == 1)
        {
            // Merge Block
            prev->size_block += current->size_block;
            prev->next = current->next;
        }
    }


}

void* realloc(void* ptr, size_t size) {
    if( size <= 0 || !ptr )
        return 0;

    memory_block_t* block = (memory_block_t*)( ptr - sizeof( memory_block_t ));
    if( block->size_block - sizeof( memory_block_t ) >= size )
        return ptr;

    void* new_ptr = malloc( size );
    memset( new_ptr, 0, size );
    memcpy(new_ptr, ptr, block->size_block - sizeof( memory_block_t ));
    free( ptr );
    return new_ptr;
}