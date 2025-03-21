#include <fs/fat.h>
#include <driver/disk_driver.h>
#include <driver/vga_driver.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

#define BYTES_SECTOR 512

struct FAT_Header fat_header;
uint32_t fat_start_sector = 0;
uint32_t fat_data_sector = 0;
char buffer[512] = {0};

uint32_t find_empty_cluster();
uint32_t create_next_cluster(uint32_t cluster);
void update_cluster(addr_t address, uint32_t value_cluster);
uint32_t find_next_cluster(uint32_t cluster);
uint32_t find_lba_cluster_region(uint32_t cluster);

void fat_initialize() {
    disk_read(0, buffer);
    memcpy(&fat_header, buffer, sizeof( struct FAT_Header ));

    fat_start_sector = fat_header.reserved_sectors;
    fat_data_sector = fat_start_sector + fat_header.fat_numbers * fat_header.sectors_per_fat;
}

struct FAT_Header* get_fat_header() {
    return &fat_header;
}

FILE* find_file(const char* filename) {

    FILE* target = malloc( sizeof( FILE ) );
    memset( target, 0, sizeof(FILE) );

    struct FAT_Header* header = get_fat_header();

    uint32_t cluster = header->cluster;
    uint32_t fat_region = header->reserved_sectors;
    uint32_t data_region =  fat_region + ( header->fat_numbers * header->sectors_per_fat );

    string_array_t* __dirsplit = str_split(filename, '/');
    char* buffer = (char*) malloc( header->bytes_Per_sector );

    for(int i = 0; i < __dirsplit->size; i++)
    {
        char* _filepath = str_array_get(__dirsplit, i);
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
                FileDir* dir = &((FileDir*) buffer)[i];

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

                            if (j == 8 && !(dir->attribute & FAT_DIRECTORY))
                                _file_tmp_name[_length_tmp++] = '.';
                            
                            _file_tmp_name[_length_tmp++] = tolowercase(dir->filename[j]);
                        }

                    } else {
                        strreverse(_file_tmp_name);
                    }

                    
                    if( strcmp(_file_tmp_name, _filepath) == 0 )
                    {
                        
                        //memcpy(target, dir, sizeof( FileDir ));
                        target->size_char = strlen( _file_tmp_name );
                        target->filename = (char*) malloc( target->size_char + 1 );
                        memcpy(target->filename, _file_tmp_name, target->size_char + 1);
                        target->flags = dir->attribute;
                        target->low_cluster = dir->low_cluster;
                        target->high_cluster = dir->high_cluster;
                        target->creation_time = dir->creation_time;
                        target->creation_date = dir->creation_date;
                        target->last_modification_time = dir->last_modification_time;
                        target->last_modification_date = dir->last_modification_date;
                        target->file_size = dir->file_size;
                        
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
    }

    str_array_clear(__dirsplit);
    free(__dirsplit);

    if( target->low_cluster == 0)
    {
        printf("File Not Found\n");
        free( buffer );
        free( target );
        return NULL;
    }
    
    free( buffer );
    return target;
}
void read_file(FILE* file, void* _dest) {
    uint32_t cluster = file->high_cluster << 16 | file->low_cluster;
    
    while( cluster < 0x0FFFFFFF )
    {
        int cluster_region = fat_data_sector + (cluster - 2) * fat_header.sectors_per_cluster;

        disk_read(cluster_region, _dest);
        _dest += fat_header.bytes_Per_sector;
        
        int FAT_Seg = (int)( cluster / 128 );
        int FAT_Off = cluster % 128;
        disk_read(fat_start_sector + FAT_Seg, buffer);
        cluster = ((uint32_t*) buffer)[FAT_Off];
    }
}

FILE* make_dir(const char* filename) {
    return make_entry(filename, FAT_DIRECTORY, NULL, 0);
}

FILE* make_entry(const char* filename, uint8_t flags, char* __buffer, size_t size) {
    FILE* dir = (FILE*) malloc( sizeof( FILE ));
    memset(dir, 0, sizeof( FILE ));
    
    dir->flags = flags;
    dir->file_size = size;

    string_array_t* __filepathspt = str_split(filename, '/');
    char* __filename = str_array_get(__filepathspt, __filepathspt->size - 1);
    char* buffer = (char*) malloc( 512 );

    char* path_join = strsubst(filename, 0, strlen(filename) - strlen(__filename));
    uint32_t cluster = fat_header.cluster;
    
    if( path_join != NULL ) {
        path_join[ strlen(path_join) - 1] = 0;
        FILE* parent_dir = find_file(path_join);
        if( parent_dir == NULL )
        {
            printf("Error> %s is not exists\n", path_join);

            free( path_join );
            str_array_clear( __filepathspt );
            free( __filepathspt );
            free( dir );
            return NULL;
        }

        if( !(parent_dir->flags & FAT_DIRECTORY) )
        {
            printf("Error> %s is not a Directory\n", path_join);
            free( path_join );
            str_array_clear( __filepathspt );
            free( __filepathspt );
            free( dir );
            return NULL;
        }

        cluster = (parent_dir->high_cluster << 16) | parent_dir->low_cluster;
    }
    
    uint32_t EOF = (flags & FAT_DIRECTORY) ? END_OF_FOLDER : END_OF_FILE;

    int dir_entries = 1;
    FileDir* __dir = (FileDir*) malloc( sizeof( FileDir ));
    memset(__dir, 0, sizeof( FileDir ));
    FileDirLongName* lfns = NULL;
    int _index = 0;
    int l = strlen(__filename);
    int lfn_entries = (strlen( __filename ) / 13) + 1;

    if( l >= 11 )
    {
        dir_entries += lfn_entries;
        lfns = (FileDirLongName*) malloc( ((l / 13) + 1) * sizeof( FileDirLongName ) );
    }

    dir->file_size = l;
    dir->filename = (char*) malloc( l + 1);
    memcpy(dir->filename, __filename, l + 1);

    while(_index <= l && l >= 11)
    {
        int j = (int)(_index / 13);
        FileDirLongName* lfn = &lfns[j];
        memset(lfn, 0, sizeof( FileDirLongName ));
        lfn->attribute = FAT_LFN;

        for(int i = 0; i < 5; i++)
        {
            if( _index > l)
                lfn->first_char[i] = 0xFFFF;
            else
                lfn->first_char[i] = __filename[_index++];
        }

        for(int i = 0; i < 6; i++)
        {
            if( _index > l)
                lfn->second_char[i] = 0xFFFF;
            else
                lfn->second_char[i] = __filename[_index++];
        }

        for(int i = 0; i < 2; i++)
        {
            if( _index > l)
                lfn->third_char[i] = 0xFFFF;
            else
                lfn->third_char[i] = __filename[_index++];
        }
    }
    
    for(int i = 0; i < 11; i++)
    {
        __dir->filename[i] = touppercase(__filename[i]);
    }
    
    
    // Create & Write File Dir Entry
    while( cluster < END_OF_FOLDER )
    {
        uint32_t creg = find_lba_cluster_region( cluster );
        disk_read(creg, buffer);
        
        for(int i = 0; i < 16; i++)
        {
            FileDir* d = &((FileDir*) buffer)[i];
            if( d->attribute != 0 )
                continue;
            
            if( dir_entries == 0 )
                break;
                
            if( dir_entries > 1 )
            {
                ((FileDirLongName*) buffer)[i] = lfns[dir_entries - 2];
            }
            else {
                // Find new cluster
                uint32_t new_cluster = find_empty_cluster();
                int sector = (int)(new_cluster / 128);
                int offset = new_cluster % 128;
                // Update new cluster
                update_cluster((fat_header.reserved_sectors + sector) * 512 + offset * sizeof( uint32_t ), END_OF_FOLDER);

                // Set new cluster
                dir->high_cluster = (new_cluster >> 16) & 0xFF;
                dir->low_cluster = new_cluster & 0xFF;
                __dir->high_cluster = (new_cluster >> 16) & 0xFF;
                __dir->low_cluster = new_cluster & 0xFF;
                __dir->attribute = flags;
                // drop it into buffer
                ((FileDir*) buffer)[i] = *__dir;
            }
                
            dir_entries--;
            // write disk
            disk_write(creg, (char*) buffer);
        }

        if( dir_entries == 0 )
            break;

        uint32_t prev_cluster = cluster;
        cluster = find_next_cluster( cluster );
        // Expanding space by create new cluster when entries is not empty
        if( cluster == END_OF_FOLDER && dir_entries != 0 )
        {
            int sector = (int)(prev_cluster / 128);
            int offset = prev_cluster % 128;
            cluster = find_empty_cluster();

            int nsector = (int)(cluster / 128);
            int noffset = cluster % 128;

            update_cluster(( fat_header.reserved_sectors + sector) * 512 + offset * sizeof( uint32_t ), cluster);
            update_cluster(( fat_header.reserved_sectors + nsector) * 512 + noffset * sizeof( uint32_t ), END_OF_FOLDER);
        }
    }

    // Write Data inside of Disk
    if( buffer != NULL && size > 0 )
    {
        cluster = (dir->low_cluster) | (dir->high_cluster << 16);

        while( cluster < EOF )
        {
            uint32_t cluster_region = find_lba_cluster_region( cluster );
            if( size >= BYTES_SECTOR )
            {
                disk_write(cluster_region, __buffer);
                size -= 512;
            }
            else {
                memset(buffer, 0, BYTES_SECTOR );
                memcpy(buffer, __buffer, size);
                disk_write(cluster_region, buffer);
                size = 0;
            }

            uint32_t prev_cluster = cluster;
            cluster = find_next_cluster( cluster );
            // Expanding space by create new cluster when entries is not empty
            if( cluster == EOF && size > 0 )
            {
                int sector = (int)(prev_cluster / 128);
                int offset = prev_cluster % 128;
                cluster = find_empty_cluster();

                int nsector = (int)(cluster / 128);
                int noffset = cluster % 128;

                update_cluster(( fat_header.reserved_sectors + sector) * 512 + offset * sizeof( uint32_t ), cluster);
                update_cluster(( fat_header.reserved_sectors + nsector) * 512 + noffset * sizeof( uint32_t ), EOF);
            }
        }
    }

    free( __dir );
    free( lfns );

    free( buffer );
    free( __filename );
    free(path_join);
    return dir;
}

int read_next_dir(DIR* __dir, FILE* __output) {
    if( __dir->parent == NULL )
    {
        if( __dir->cluster == 0 )
        __dir->cluster = fat_header.cluster;
    }
    
    
    char* buffer = (char*) malloc( BYTES_SECTOR );
    string_t __filename;
    __filename.size = 0;
    __filename.buffer = NULL;
    while( __dir->cluster < END_OF_FOLDER )
    {
        uint32_t cluster_region = find_lba_cluster_region( __dir->cluster );
        disk_read(cluster_region, buffer);
        for(int i = __dir->index % 16; i < 16; i++)
        {
            FileDir* file = &((FileDir*) buffer)[i];
            if( file->attribute == 0 )
            {
                __dir->index++;
                continue;
            }

            if( file->attribute == FAT_LFN )
            {
                FileDirLongName* lfn = (FileDirLongName*) file;

                for(int m = 1; m >= 0; m--)
                {
                    if( lfn->third_char[m] == 0x0000 || lfn->third_char[m] == 0xFFFF)
                        continue;
                    
                    char _v[2] = {lfn->third_char[m], 0};
                    str_push(&__filename, _v);
                }

                for(int m = 5; m >= 0; m--)
                {
                    if( lfn->second_char[m] == 0x0000 || lfn->second_char[m] == 0xFFFF)
                        continue;

                    char _v[2] = {lfn->second_char[m], 0};
                    str_push(&__filename, _v);
                }

                for(int m = 4; m >= 0; m--)
                {
                    if( lfn->first_char[m] == 0x0000 || lfn->first_char[m] == 0xFFFF)
                        continue;

                    char _v[2] = {lfn->first_char[m], 0};
                    str_push(&__filename, _v);
                }

                __dir->index++;
                continue;
            }

            
            if( __filename.size == 0 )
            {
                for(int m = 0; m < 11; m++)
                {
                    char _v[2] = {tolowercase(file->filename[m]), 0};
                    if( m == 8 && (file->attribute & FAT_DIRECTORY) )
                    {
                        _v[0] = '.';
                        str_push(&__filename, _v);
                        _v[0] = tolowercase(file->filename[m]);
                    }
                    
                    str_push(&__filename, _v);
                }
                
                
                __output->file_size = __filename.size;
                __output->filename = calloc(__filename.buffer);
                __output->high_cluster = file->high_cluster;
                __output->low_cluster = file->low_cluster;
                __output->creation_time = file->creation_time;
                __output->creation_date = file->creation_date;
                __output->last_modification_time = file->last_modification_time;
                __output->last_modification_date = file->last_modification_date;
                __dir->index++;
                str_clear(&__filename);
                free( buffer );
                return 0;
            }
            else if( __filename.size > 0 )
            {
                strreverse(__filename.buffer);
                __output->file_size = __filename.size;
                __output->filename = calloc(__filename.buffer);
                __output->high_cluster = file->high_cluster;
                __output->low_cluster = file->low_cluster;
                __output->creation_time = file->creation_time;
                __output->creation_date = file->creation_date;
                __output->last_modification_time = file->last_modification_time;
                __output->last_modification_date = file->last_modification_date;
                __dir->index++;
                str_clear(&__filename);
                free( buffer );
                return 0;
            }

        }
        __dir->cluster = find_next_cluster(__dir->cluster);
    }
    free( buffer );
    return 1;
}

DIR* open_dir(const char* filename) {
    DIR* dir = (DIR*) malloc( sizeof( DIR ) );
    memset(dir, 0, sizeof( DIR ));
    
    if( strlen(filename) > 0 )
    {
        FILE* file = find_file(filename);
        dir->parent = file;
    }
    else {
        dir->parent = NULL;
    }
    dir->index = 0;
    dir->cluster = 0;
    return dir;
}

uint32_t find_empty_cluster() {
    uint32_t* buffer = (uint32_t*) malloc( 512 );
    uint32_t indexFAT = 0;
    while( indexFAT < fat_header.sectors_per_fat )
    {
        disk_read(fat_header.reserved_sectors + indexFAT, buffer);
        for(int i = 0; i < fat_header.bytes_Per_sector / sizeof( uint32_t ); i++)
        {
            if( buffer[i] == 0 )
            {
                free( buffer );
                return indexFAT * (fat_header.bytes_Per_sector / sizeof( uint32_t )) + i;
            }
        }
        indexFAT++;
    }
    free( buffer );
    return 0;
}

uint32_t find_next_cluster(uint32_t cluster) {
    char* b = (char*) malloc( 512 );
    int i = (int)(cluster / (fat_header.bytes_Per_sector / sizeof( uint32_t )));
    int j = (cluster % (fat_header.bytes_Per_sector / sizeof( uint32_t )));
    disk_read(fat_header.reserved_sectors + i, b);
    int c = b[j];
    free( b );
    return c;
}
uint32_t find_lba_cluster_region(uint32_t cluster) {
    return (fat_header.reserved_sectors + ( fat_header.fat_numbers * fat_header.sectors_per_fat)) + ( cluster - 2 ) * fat_header.sectors_per_cluster;
}

uint32_t create_next_cluster(uint32_t cluster) {
    uint32_t empty_cluster = find_empty_cluster();
}

void update_cluster(addr_t address, uint32_t value_cluster) {
    uint32_t sector = (int)(address / fat_header.bytes_Per_sector);
    uint32_t offset = address % fat_header.bytes_Per_sector;
    uint32_t* buffer = (uint32_t*) malloc( BYTES_SECTOR );
    disk_read(sector, buffer);
    buffer[(int)(offset / sizeof(uint32_t))] = value_cluster;
    disk_write(sector, buffer);
    free(buffer);
}