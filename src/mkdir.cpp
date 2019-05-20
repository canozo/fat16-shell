#include <sstream>
#include <vector>
#include <stdio.h>
#include "dir_entry.h"
#include "mkdir.h"

using std::stringstream;
using std::vector;

string mkdir(FILE *file, part_table_t *fat_pt, boot_sector_t *bs, fat_utils_t *utils, string cd, string dir) {
  // devuelve un mensaje de estado
  stringstream res("");

  if (cd == "/") {
    // seek al inicio del dir root
    fseek(file, utils->root_start, SEEK_SET);

    bool exists = false;
    dir_entry_t entry;
    for (int i = 0; i < bs->root_dir_entries; i += 1) {
      // buscar si ya existe el directorio
      fread(&entry, sizeof(entry), 1, file);

      if (compare_dir_name(&entry, dir)) {
        // ya existe
        exists = true;
        res << "Ya existe el directorio [" << dir << "] en el directorio [" << cd << "].\n";
        break;
      }
    }

    // si no existe, crearlo
    if (!exists) {
      // para crear un directorio en el root ocupamos:
      bool found_entry = false; // un dir entry vacio
      bool found_cluster = false; // un cluster vacio

      // buscar un dir entry vacio en el root
      fseek(file, utils->root_start, SEEK_SET);
      int entry_num;
      for (entry_num = 0; entry_num < bs->root_dir_entries; entry_num += 1) {
        fread(&entry, sizeof(entry), 1, file);

        if (entry.filename[0] == 0x00) {
          found_entry = true;
          break; // entry vacio en entry_num
        }
      }

      // buscar un cluster vacio
      unsigned short cluster;
      for (cluster = 2; cluster < 101; cluster += 1) {
        fseek(file, utils->data_start + utils->cluster_size * cluster, SEEK_SET);

        int i;
        for (i = 0; i < 512; i += 1) {
          fread(&entry, sizeof(entry), 1, file);

          if (entry.filename[0] == 0x00) {
            continue; // entry vacio
          } else {
            break; // entry no vacio
          }
        }

        if (i == 512) {
          found_cluster = true;
          break;
        }
      }

      if (found_entry && found_cluster) {
        // crear dir entry en root
      } else {
        res << "No se pudo crear el directorio [" << dir << "] en el directorio [" << cd << "].\n";
        res << "found_entry: " << found_entry << ", found_cluster: " << found_cluster << ".\n";
      }

    }
  } else {
    // no es el directorio raiz
  }
  return res.str();
}
