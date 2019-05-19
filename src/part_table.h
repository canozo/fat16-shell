#ifndef PART_TABLE_H
#define PART_TABLE_H

// partition table del disco
#pragma pack(push, 1)
typedef struct part_table_t {
  unsigned char bootable;
  unsigned char start_chs[3];
  unsigned char type;
  unsigned char end_chs[3];
  unsigned long start_sector;
  unsigned long len_sector;
} part_table_t;
#pragma pack(pop)

#endif
