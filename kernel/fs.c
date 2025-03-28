#define __SYSCALL__FUNCTION__
#include <fs/fat.h>
#include <fs/fs.h>
#include <syscall.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <driver/disk_driver.h>

#define MAX_FILE 1024

file_t* __fa[MAX_FILE];

DWORD fopen(const char* filename, const char* flags)
{
    return sys_open( filename, flags );
}
void fclose(DWORD __fd)
{
    sys_close(__fd);
}
void fseek(DWORD __fd, long position) 
{

}
long ftell(DWORD __fd) 
{

}
int fread(DWORD __fd, char* __buffer, size_t __size) 
{
    sys_read(__fd, (uint8_t*) __buffer, __size);
    return 0;
}

int fwrite(DWORD __fd, char* __buffer, size_t __size) 
{

}

DWORD   sys_open(const char* __filename, const char* __flags) {
    file_t* fd = malloc( sizeof( file_t ) );
    memset( fd, 0, sizeof(file_t) );

    int idx_fa = 3;
    for(idx_fa = 3; idx_fa < MAX_FILE; idx_fa++)
    {
        if( __fa[idx_fa] == 0 )
        {
            __fa[idx_fa] = fd;
            break;
        }
    }

    if( idx_fa == MAX_FILE )
    {
        free(fd);
        return -1;
    }

    struct FAT_Header* header = get_fat_header();

    uint32_t cluster = header->cluster;
    uint32_t fat_region = header->reserved_sectors;
    uint32_t data_region =  fat_region + ( header->fat_numbers * header->sectors_per_fat );

    struct FATDirectory* target_dir = (struct FATDirectory*) malloc( sizeof( struct FATDirectory ) );
    memset(target_dir, 0, sizeof( struct FATDirectory ));

    char* file_path = __filename;
    int index = 0;
    uint8_t* buffer = (uint8_t*) malloc( 512 );
    while( file_path[index] != 0 )
    {
        // Getting name of path
        char* _uname = (char*) malloc(1);
        int _len = 0;
        while( file_path[index] != '/' && file_path[index] != 0 )
        {
            _uname[_len] = file_path[index];
            _len++;
            _uname = realloc((void*) _uname, _len + 1);
            index++;
        }
        _uname[_len] = 0;
        
        

        char* _file_tmp_name = (char*) malloc( 256 );
        memset(_file_tmp_name, 0, 256);
        int _length_tmp = 0;
        bool find_file = 0;
        while( cluster < END_OF_FOLDER )
        {
            uint32_t c_region = data_region + (cluster - 2) * header->sectors_per_cluster;
            disk_read(c_region, buffer);

            for(int i = 0; i < 16; i++)
            {
                struct FATDirectory* dir = &((struct FATDirectory*) buffer)[i];

                if( dir->attribute == FAT_LFN )
                {
                    FileDirLongName* lfn = (FileDirLongName*) dir;
                    for(int m = 1; m >= 0; m--)
                    {
                        if( lfn->third_char[m] == 0x0000 || lfn->third_char[m] == 0xFFFF)
                            continue;

                        _file_tmp_name[_length_tmp++] = lfn->third_char[m];
                    }

                    for(int m = 5; m >= 0; m--)
                    {
                        if( lfn->second_char[m] == 0x0000 || lfn->second_char[m] == 0xFFFF)
                            continue;

                        _file_tmp_name[_length_tmp++] = lfn->second_char[m];
                    }

                    for(int m = 4; m >= 0; m--)
                    {
                        if( lfn->first_char[m] == 0x0000 || lfn->first_char[m] == 0xFFFF)
                            continue;

                        _file_tmp_name[_length_tmp++] = lfn->first_char[m];
                    }


                }
                else {
                    if( _length_tmp == 0 )
                    {
                        _length_tmp = 0;
                        for(int j = 0; j < 11; j++)
                        {
                            if(dir->filename[j] == ' ')
                                continue;

                            if (j == 8)
                                _file_tmp_name[_length_tmp++] = '.';
                            
                            _file_tmp_name[_length_tmp++] = tolowercase(dir->filename[j]);
                        }

                    } else {
                        strreverse(_file_tmp_name);
                    }

                    
                    if( strcmp(_file_tmp_name, _uname) == 0 )
                    {
                        
                        memcpy(target_dir, dir, sizeof( struct FATDirectory ));
                        memcpy(fd->name, _file_tmp_name, 256);
                        find_file = 1;

                        memset(_file_tmp_name, 0, 256);
                        _length_tmp = 0;
                        break;
                    } else {
                        memset(_file_tmp_name, 0, 256);
                        _length_tmp = 0;
                    }
                }
                
            }

            if( find_file )
                break;

            uint32_t fat_seg = (int)(cluster / 128);
            uint32_t fat_off = cluster % 128;
            disk_read(fat_region + fat_seg, buffer);
            cluster = ((uint32_t*) buffer)[fat_off];
        }
        free(_file_tmp_name);
        free(_uname);
    }

    if( target_dir->low_cluster == 0)
    {
        printf("File Not Found\n");
        return -1;
    }

    fd->start_position = (data_region + ( ((target_dir->high_cluster << 16) | target_dir->low_cluster) - 2 ) * header->sectors_per_cluster) * 512;
    fd->position = 0;
    fd->size = target_dir->file_size;
    
    free( target_dir );
    free( buffer );
    return idx_fa;
}
VOID    sys_close(DWORD __fd) {
    uint32_t i = (uint32_t) __fa[__fd];
    file_t *file = (file_t*) i;
    
    free( file );
}
VOID    sys_read(DWORD __fd, BYTE* __buffer, size_t __size) {
    if( __fd < 3 || __fd >= MAX_FILE )
        return;
    
    uint32_t i = (uint32_t) __fa[__fd];
    file_t *fd = (file_t*) i;
    uint8_t* buffer = (uint8_t*) __buffer;
    size_t count = __size;

    // Get the FAT Header
    struct FAT_Header* header = get_fat_header();
    uint32_t fat_region = header->reserved_sectors;
    uint32_t data_region =  fat_region + ( header->fat_numbers * header->sectors_per_fat );

    uint8_t* fatbuffer = (uint8_t*) malloc( header->bytes_Per_sector );
    uint8_t* databuffer = (uint8_t*) malloc( header->bytes_Per_sector );
    
    uint32_t cluster = ((fd->start_position / 512 - data_region) / header->sectors_per_cluster) + 2;

    for(int i = 0; i < (int)( fd->position / 512 ); i++)
    {
        uint32_t fat_seg = (int)(cluster / 128);
        uint32_t fat_off = cluster % 128;
        disk_read(fat_region + fat_seg, fatbuffer);
        cluster = ((uint32_t*) fatbuffer)[fat_off];
    }

    int index_cnt = 0;
    while( cluster < END_OF_FOLDER )
    {
        uint32_t creg = data_region + ( cluster - 2 ) * header->sectors_per_cluster;
        disk_read(creg, databuffer);

        for(int i = fd->position % 512; i < 512; i++)
        {
            if( count == 0 )
                break;

            buffer[index_cnt++] = databuffer[i];
            count--;
            fd->position++;
        }

        // if( count >= 512 )
        // {
        //     buffer += 512;
        //     count -= 512;
        //     fd->position += 512;
        // } else {
        //     disk_read(creg, databuffer);
        //     uint32_t d = fd->position % 512;

        //     memcpy(buffer, databuffer, count);
        //     fd->position += count;
        //     count = 0;
        // }

        uint32_t fat_seg = (int)(cluster / 128);
        uint32_t fat_off = cluster % 128;
        disk_read(fat_region + fat_seg, fatbuffer);
        cluster = ((uint32_t*) fatbuffer)[fat_off];
    }
    free(fatbuffer);
    free(databuffer);
}

VOID    sys_write(DWORD __fd, BYTE* __buffer, size_t __size) {

}

DWORD   sys_getsize(DWORD __fd) {

    // Ignore All system fd
    if( __fd < 3 || __fd >= MAX_FILE )
        return NULL;

    uint32_t i = (uint32_t) __fa[__fd];
    file_t *fd = (file_t*) i;
    return fd->size;
}

DWORD   sys_mkdir(const char* __dirname) {
    FILE* dir = make_dir(__dirname);
    if( dir >= 0x00100000 )
        free( dir );
    return (uint32_t) dir;
}

DWORD   sys_mkfile(const char* __filename, char* buffer, size_t size) {
    FILE* file = make_entry(__filename, FAT_ARCHIVE, buffer, size);
    if( file >= 0x00100000 )
        free( file );
    return (uint32_t) file;
}

DWORD   sys_opendir(const char* __dirname) {
    DIR* dir = open_dir(__dirname);

    // Malloc dir_t
    dir_t* fd = malloc( sizeof( dir_t ) );
    memset( fd, 0, sizeof(dir_t) );

    fd->parent_dir = (addr_t) calloc(dir->parent);
    fd->cluster = dir->cluster;
    fd->index = dir->index;
    
    // Free dir
    free( dir->parent );
    free( dir );

    // Find index
    int idx_fa = 3;
    for(idx_fa = 3; idx_fa < MAX_FILE; idx_fa++)
    {
        if( __fa[idx_fa] == 0 )
        {
            __fa[idx_fa] = (file_t*) fd;
            break;
        }
    }

    if( idx_fa == MAX_FILE )
    {
        free( fd );
        return -1;
    }

    return idx_fa;
}
VOID    sys_closedir(DWORD __fd) {
    if( __fd < 3 || __fd >= MAX_FILE )
        return;

    dir_t* d = (dir_t*) ((uint32_t) __fa[__fd]);
    free( ((FILE*) d->parent_dir)->filename );
    free( (FILE*) d->parent_dir );
    free( d );
    __fa[__fd] == NULL;
}

DWORD   sys_readdir(DWORD __fd, FILE* __output) {
    if( __fd < 3 || __fd >= MAX_FILE )
        return 1;

    dir_t* dir = (dir_t*) ((uint32_t) __fa[__fd]);
    DIR d;
    d.parent = (FILE*) dir->parent_dir;
    d.index = dir->index;
    d.cluster = dir->cluster;

    uint32_t r = read_next_dir(&d, __output);
    dir->index = d.index;
    dir->cluster = d.cluster;
    return r;
}