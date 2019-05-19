#include <sstream>
#include "dir_entry.h"
#include "ls.h"

using std::stringstream;

string file_info(dir_entry_t *entry) {
  stringstream res("");
  char namebuff[9];
  char extbuff[3];

  // tipo de archivo
  switch(entry->filename[0]) {
    case 0x00:
      // entry vacio
      break;

    case 0xE5:
      res << "Archivo borrado: ";
      break;

    case 0x05:
      res << "Archivo comenzando con 0xE5: ";
      break;

    case 0x2E:
      res << "Directorio \".\" o \"..\": ";
      break;

    default:
      res << "Archivo o subdirectorio: ";
      break;
  }

  // metadata del archivo
  switch(entry->filename[0]) {
    case 0x00:
      // entry vacio
      break;

    case 0xE5:
    case 0x05:
    case 0x2E:
    default:
      memcpy(namebuff, &entry->filename[0], 8);
      memcpy(extbuff, &entry->ext[0], 3);

      res << namebuff << '.' << extbuff << '\n';

      // fecha: dd-MM-yy
      int dd, MM, yy;
      yy = 1980 + (entry->modify_date >> 9);
      MM = (entry->modify_date >> 5) & 0xF;
      dd = entry->modify_date & 0x1F;

      // hora: hh-mm-ss
      int hh, mm, ss;
      hh = (entry->modify_time >> 11);
      mm = (entry->modify_time >> 5) & 0x3F;
      ss = entry->modify_time & 0x1F;

      res << "Fecha de modificacion: " << dd << '-' << MM << '-' << yy;
      res << ' ' << hh << ':' << mm << ':' << ss << '\n';

      res << "Cluster de inicio: " << entry->starting_cluster << '\n';
      res << "Tamanio: " << (int) (entry->file_size / 1024) << "KB\n";
      res << '\n';
      break;
  }

  return res.str();
}

string ls(FILE *file, part_table_t *fat_pt, boot_sector_t *bs, string cd) {
  stringstream res("");

  if (cd == "/") {
    // seek al inicio del boot sector
    fseek(file, 512 * fat_pt->start_sector, SEEK_SET);
    // seek a los dir entries en root
    long int offset = (bs->reserved_sectors - 1 + bs->fat_size_sectors * bs->number_of_fats) * bs->sector_size;
    fseek(file, offset, SEEK_CUR);

    dir_entry_t entry;
    for (int i = 0; i < bs->root_dir_entries; i += 1) {
      fread(&entry, sizeof(entry), 1, file);
      res << file_info(&entry);
    }
  }

  return res.str();
}
