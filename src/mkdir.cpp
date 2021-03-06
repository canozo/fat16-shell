#include <sstream>
#include <vector>
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
      fread(&entry, sizeof(dir_entry_t), 1, file);

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
        fread(&entry, sizeof(dir_entry_t), 1, file);

        if (entry.filename[0] == 0x00) {
          found_entry = true;
          break; // entry vacio en entry_num
        }
      }

      // buscar un cluster vacio
      unsigned short cluster;
      // TODO posible bug, utilizar += 2
      for (cluster = 2; cluster < 101; cluster += 1) {
        fseek(file, utils->data_start + utils->cluster_size * cluster, SEEK_SET);

        int i;
        for (i = 0; i < 512; i += 1) {
          fread(&entry, sizeof(dir_entry_t), 1, file);

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
        dir_entry_t new_entry;
        init_de_directory(&new_entry);

        // copiar dir al filename
        string new_filename = get_dir_name(dir);
        memcpy(new_entry.filename, new_filename.c_str(), new_filename.size());

        // el valor de cluster inicial es cluster + 2, pero se escribe en cluster
        new_entry.starting_cluster = cluster + 2;

        // ir a la posicion del dir entry vacio y escribir el dir entry
        fseek(file, utils->root_start + entry_num * sizeof(dir_entry_t), SEEK_SET);
        fwrite(&new_entry, sizeof(dir_entry_t), 1, file);

        // ir a la posicion del cluster vacio y escribir el cluster
        fseek(file, utils->data_start + utils->cluster_size * cluster, SEEK_SET);

        // como es un directorio, hay que escibir la tabla de dir entries
        // escribir .
        memcpy(new_entry.filename, ".       ", 8);
        fwrite(&new_entry, sizeof(dir_entry_t), 1, file);

        // escribir ..
        dir_entry_t parent;
        init_de_root(&parent);
        fwrite(&parent, sizeof(dir_entry_t), 1, file);

      } else {
        res << "No se pudo crear el directorio [" << dir << "] en el directorio [" << cd << "].\n";
        res << "found_entry: " << found_entry << ", found_cluster: " << found_cluster << ".\n";
      }
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
    dir_entry_t parent = entry;

    bool exists = false;
    for (int i = 0; i < 512; i += 1) {
      // buscar si ya existe el directorio
      fread(&entry, sizeof(dir_entry_t), 1, file);

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
      fseek(file, utils->data_start + utils->cluster_size * (parent.starting_cluster - 2), SEEK_SET);
      int entry_num;
      for (entry_num = 0; entry_num < 512; entry_num += 1) {
        fread(&entry, sizeof(dir_entry_t), 1, file);

        if (entry.filename[0] == 0x00) {
          found_entry = true;
          break; // entry vacio en entry_num
        }
      }

      // buscar un cluster vacio
      unsigned short cluster;
      // TODO posible bug, utilizar += 2
      for (cluster = 2; cluster < 101; cluster += 1) {
        fseek(file, utils->data_start + utils->cluster_size * cluster, SEEK_SET);

        int i;
        for (i = 0; i < 512; i += 1) {
          fread(&entry, sizeof(dir_entry_t), 1, file);

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
        dir_entry_t new_entry;
        init_de_directory(&new_entry);

        // copiar dir al filename
        string new_filename = get_dir_name(dir);
        memcpy(new_entry.filename, new_filename.c_str(), new_filename.size());

        // el valor de cluster inicial es cluster + 2, pero se escribe en cluster
        new_entry.starting_cluster = cluster + 2;

        // ir a la posicion del dir entry vacio y escribir el dir entry
        fseek(file, utils->data_start + utils->cluster_size * (parent.starting_cluster - 2) + entry_num * sizeof(dir_entry_t), SEEK_SET);
        fwrite(&new_entry, sizeof(dir_entry_t), 1, file);

        // ir a la posicion del cluster vacio y escribir el cluster
        fseek(file, utils->data_start + utils->cluster_size * cluster, SEEK_SET);

        // como es un directorio, hay que escibir la tabla de dir entries
        // escribir .
        memcpy(new_entry.filename, ".       ", 8);
        fwrite(&new_entry, sizeof(dir_entry_t), 1, file);

        // escribir ..
        memcpy(parent.filename, "..      ", 8);
        fwrite(&parent, sizeof(dir_entry_t), 1, file);

      } else {
        res << "No se pudo crear el directorio [" << dir << "] en el directorio [" << cd << "].\n";
        res << "found_entry: " << found_entry << ", found_cluster: " << found_cluster << ".\n";
      }
    }
  }

  return res.str();
}
