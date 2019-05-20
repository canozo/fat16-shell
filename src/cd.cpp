#include <sstream>
#include "dir_entry.h"
#include "cd.h"

using std::stringstream;

string cd(FILE *file, part_table_t *fat_pt, boot_sector_t *bs, fat_utils_t *utils, string cd, string dest) {
  // devuelve el directorio o ruta al que se movio
  stringstream res("");
  res << cd;

  // ver si el archivo se encuentra en el cd
  if (cd == "/") {
    // seek al inicio del dir root
    fseek(file, utils->root_start, SEEK_SET);

    dir_entry_t entry;
    string entry_fn;
    for (int i = 0; i < bs->root_dir_entries; i += 1) {
      // buscar en cada uno de los entries
      fread(&entry, sizeof(entry), 1, file);

      if (compare_dir_name(&entry, dest)) {
        // lo encontro, wey
        res << get_dir_name(dest);
        break;
      }
    }
  } else {
    // no es el directorio raiz
  }

  return res.str();
}
