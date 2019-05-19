#include <sstream>
#include "dir_entry.h"
#include "ls.h"

using std::stringstream;

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
  } else {
    // no es el directorio raiz
  }

  return res.str();
}
