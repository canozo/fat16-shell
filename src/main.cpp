#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "part_table.h"
#include "boot_sector.h"
#include "dir_entry.h"

using std::cin;
using std::cout;

void print_file_info(dir_entry_t *entry) {
    switch(entry->filename[0]) {
    case 0x00:
        return; // unused entry
    case 0xE5:
        printf("Deleted file: [?%.7s.%.3s]\n", entry->filename+1, entry->ext);
        return;
    case 0x05:
        printf("File starting with 0xE5: [%c%.7s.%.3s]\n", 0xE5, entry->filename+1, entry->ext);
        break;
    case 0x2E:
        printf("Directory: [%.8s.%.3s]\n", entry->filename, entry->ext);
        break;
    default:
        printf("File: [%.8s.%.3s]\n", entry->filename, entry->ext);
    }

    printf("  Modified: %04d-%02d-%02d %02d:%02d.%02d    Start: [%04X]    Size: %d\n",
        1980 + (entry->modify_date >> 9), (entry->modify_date >> 5) & 0xF, entry->modify_date & 0x1F,
        (entry->modify_time >> 11), (entry->modify_time >> 5) & 0x3F, entry->modify_time & 0x1F,
        entry->starting_cluster, entry->file_size);
}

int main(int argc, char *argv[]) {
  FILE *file;
  char *filename;
  part_table_t ptable[4];

  if (argc == 1) {
    filename = "fat16.img";
  } else {
    filename = argv[1];
  }

  // abrir archivo
  file = fopen(filename, "rb");
  // seek a la tabla de particiones
  fseek(file, 0x1BE, SEEK_SET);
  // leer las 4 entradas de la tabla de particiones
  fread(ptable, sizeof(part_table_t), 4, file);

  int i;
  for (i = 0; i < 4; i += 1) {
    if (ptable[i].type == 4 || ptable[i].type == 6 || ptable[i].type == 14) {
      // se encontro el FAT
      break;
    }
  }

  if (i == 4) {
    cout << "No se encontro la FAT.\n";
    fclose(file);
    return 0;
  }

  part_table_t fat_pt = ptable[i];
  boot_sector_t bs;

  // seek al inicio del boot sector
  fseek(file, 512 * fat_pt.start_sector, SEEK_SET);
  // leer la data del boot sector
  fread(&bs, sizeof(boot_sector_t), 1, file);

  long int offset = (bs.reserved_sectors - 1 + bs.fat_size_sectors * bs.number_of_fats) * bs.sector_size;
  fseek(file, offset, SEEK_CUR);

  dir_entry_t entry;
  for (int j = 0; j < bs.root_dir_entries; j += 1) {
    fread(&entry, sizeof(entry), 1, file);
    print_file_info(&entry);
  }

  fclose(file);
  return 0;
}
