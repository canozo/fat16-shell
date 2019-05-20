#ifndef FAT_UTILS_H
#define FAT_UTILS_H

// variables utiles
#pragma pack(push, 1)
typedef struct fat_utils_t {
  unsigned long fat_start;
  unsigned long root_start;
  unsigned long data_start;
  unsigned long cluster_size;
} fat_utils_t;
#pragma pack(pop)

#endif