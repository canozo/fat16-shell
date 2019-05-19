#ifndef DIR_ENTRY_H
#define DIR_ENTRY_H

// partition table del disco
#pragma pack(push, 1)
typedef struct dir_entry_t {
    unsigned char filename[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short modify_time;
    unsigned short modify_date;
    unsigned short starting_cluster;
    unsigned long file_size;
} dir_entry_t;
#pragma pack(pop)

#endif
