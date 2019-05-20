#include <sstream>
#include "dir_entry.h"
#include "cat.h"

using std::stringstream;

string cat(FILE *file, part_table_t *fat_pt, boot_sector_t *bs, fat_utils_t *utils, string cd, string filename) {
  stringstream res("");
  bool found = false;

  // ver si el archivo se encuentra en el cd
  if (cd == "/") {
    // seek al inicio del dir root
    fseek(file, utils->root_start, SEEK_SET);

    dir_entry_t entry;
    string entry_fn;
    for (int i = 0; i < bs->root_dir_entries; i += 1) {
      // buscar en cada uno de los entries
      fread(&entry, sizeof(entry), 1, file);
      entry_fn = file_name(&entry);

      if (entry_fn == "") {
        continue;
      }

      if (entry_fn == filename) {
        // lo encontro, wey
        found = true;
        res << file_read(&entry, utils, file) << "\n\n";
        break;
      }
    }
  } else {
    // no es el directorio raiz
  }

  // si no se encuentra, devolver "No se encontro el archivo"
  if (!found) {
    res << "No se encontro el archivo [" << filename << "] en el directorio [" << cd << "].\n\n";
  }

  return res.str();
}
