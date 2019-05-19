#ifndef BOOT_SECTOR_H
#define BOOT_SECTOR_H

// boot sector de fat16
#pragma pack(push, 1)
typedef struct boot_sector_t {
  unsigned char jmp[3];
  char oem[8];
  unsigned short sector_size;
  unsigned char sectors_per_cluster;
  unsigned short reserved_sectors;
  unsigned char number_of_fats;
  unsigned short root_dir_entries;
  unsigned short total_sectors_short; // if zero, later field is used
  unsigned char media_descriptor;
  unsigned short fat_size_sectors;
  unsigned short sectors_per_track;
  unsigned short number_of_heads;
  unsigned long hidden_sectors;
  unsigned long total_sectors_long;

  unsigned char drive_number;
  unsigned char current_head;
  unsigned char boot_signature;
  unsigned long volume_id;
  char volume_label[11];
  char fs_type[8];
  char boot_code[448];
  unsigned short boot_sector_signature;
} boot_sector_t;
#pragma pack(pop)

#endif
