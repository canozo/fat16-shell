#include <stdio.h>

#include <sstream>
#include <vector>
#include "dir_entry.h"
#include "ls.h"

using std::stringstream;
using std::vector;

string ls(FILE *file, part_table_t *fat_pt, boot_sector_t *bs, fat_utils_t *utils, string cd) {
  stringstream res("");

  if (cd == "/") {
    // seek al inicio del dir root
    fseek(file, utils->root_start, SEEK_SET);

    dir_entry_t entry;
    for (int i = 0; i < bs->root_dir_entries; i += 1) {
      fread(&entry, sizeof(entry), 1, file);
      res << file_info(&entry);
    }
  } else {
    // no es el directorio raiz
    vector<string> subdirs = split_path(cd);

    // empezamos con root
    fseek(file, utils->root_start, SEEK_SET);

    dir_entry_t entry;
    for (int i = 0; i < bs->root_dir_entries; i += 1) {
      fread(&entry, sizeof(entry), 1, file);

      if (compare_dir_name(&entry, subdirs[0])) {
        break; // encontro
      }
    }

    // seek al cluster del directorio
    fseek(file, utils->data_start + utils->cluster_size * (entry.starting_cluster - 2), SEEK_SET);

    // si aun hay mas subdirectorios, buscarlos:
    for (int i = 1; i < subdirs.size(); i += 1) {
      // hay 512 dir entries por cada cluster
      // uno de estos entries tiene que ser el directorio que buscamos
      for (int j = 0; j < 512; j += 1) {
        fread(&entry, sizeof(entry), 1, file);
        if (compare_dir_name(&entry, subdirs[i])) {
          break; // encontro
        }
      }

      // apuntar al nuevo cluster
      fseek(file, utils->data_start + utils->cluster_size * (entry.starting_cluster - 2), SEEK_SET);
    }

    // a este punto, llegamos al cluster del current directory
    // listamos todos los archivos
    for (int i = 0; i < 512; i += 1) {
      fread(&entry, sizeof(entry), 1, file);

      if (entry.filename[0] == 0x00) {
        continue;
      }

      res << file_info(&entry);
    }
  }

  return res.str();
}
