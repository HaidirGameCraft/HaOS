#ifndef FAT_HEADER
#define FAT_HEADER

#include <stdint.h>
#define END_OF_FOLDER 0x0FFFFFF8
#define END_OF_FILE 0x0FFFFFFF

struct FAT_Header {
    uint8_t jmp[3];
    uint8_t oem[8];
    uint16_t bytes_Per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_numbers;
    uint16_t root_dir_entries;
    uint16_t total_sectors;
    uint8_t media_type;
    uint16_t fat_reserved;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sector;

    uint32_t sectors_per_fat;
    uint16_t flags;
    uint16_t version_number;
    uint32_t cluster;
    uint16_t FSInfo;
    uint16_t backup_boot;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved_1;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t fat_identifier[8];
} __attribute__((packed));

struct FATDirectory {
    char filename[11];
    uint8_t attribute;
    uint8_t reserved_NT;
    uint8_t creation_time_milisecond;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_accessed_date;
    uint16_t high_cluster;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t low_cluster;
    uint32_t file_size;
};

typedef struct FATDirectory FileDir;

typedef struct {
    uint8_t order;
    uint16_t first_char[5];
    uint8_t attribute;
    uint8_t long_type;
    uint8_t checksum;
    uint16_t second_char[6];
    uint16_t reserved;
    uint16_t third_char[2];
} C_PACKED FileDirLongName;

typedef struct {
    uint32_t addr;
    uint32_t file_addr;
    uint16_t flags;
} FILEADDR;

struct FILE32 {
    uint16_t size_char;
    char* filename;
    uint16_t high_cluster;
    uint8_t flags;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t low_cluster;
    uint32_t file_size;
};

typedef struct FILE32 FILE;

typedef struct {
    FILE* parent;        // Parent DIR, if This empty it will read the main disk
    uint32_t cluster;
    int index;              // Index Entry
} DIR;

enum FILE_ATTR {
    FILE_ATTR_FILE = 0b00000001,
    FILE_ATTR_DIRECTORY = 0b00000010,
    FILE_ATTR_LONGNAME = 0b00100000
};

enum FATDir_ATTR {
    FAT_READ_ONLY = 0x01,
    FAT_HIDDEN = 0x02,
    FAT_SYSTEM = 0x04,
    FAT_VOLUME_ID = 0x08,
    FAT_DIRECTORY = 0x10,
    FAT_ARCHIVE = 0x20,
    FAT_LFN = 0x0F,
};

enum FATRead_ {
    ONE_BY_ONE,
    READ_ALL
};

void fat_initialize();
struct FAT_Header* get_fat_header();
#ifdef BOOT_STAGE_DEFINE
FILEADDR find_file(const char* filename);
uint32_t read_file(FILEADDR* file, void* buffer, uint32_t type);
char* read_all_file(FILEADDR* file);
#else
FILE* find_file(const char* filename);
DIR* open_dir(const char* filename);
FILE* make_dir(const char* filename);
FILE* make_entry(const char* filename, uint8_t flags, char* buffer, size_t size);
int read_next_dir(DIR* __dir, FILE* __output);
void read_file(FILE* file, void* buffer);
#endif

#endif