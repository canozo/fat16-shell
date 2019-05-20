#include <sstream>
#include "dir_entry.h"

using std::stringstream;

string file_info(dir_entry_t *entry) {
  stringstream res("");
  char namebuff[7];
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
      memcpy(namebuff, &entry->filename[0], 6);
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

string file_name(dir_entry_t *entry) {
  stringstream res("");
  char namebuff[7];
  char extbuff[3];

  switch(entry->filename[0]) {
    case 0x00:
    case 0xE5:
    case 0x05:
    case 0x2E:
      // no es archivo
      break;

    default:
      memcpy(namebuff, &entry->filename[0], 6);
      memcpy(extbuff, &entry->ext[0], 3);
      res << namebuff << '.' << extbuff;
      break;
  }

  return res.str();
}

string file_read(dir_entry_t *entry, fat_utils_t *utils, FILE *file) {
  stringstream res("");
  unsigned char buffer[4096];

  unsigned long fat_start = utils->fat_start;
  unsigned long data_start = utils->data_start;
  unsigned long cluster_size = utils->cluster_size;
  unsigned short cluster = entry->starting_cluster;

  size_t bytes_read;
  size_t bytes_to_read;
  size_t file_left = entry->file_size;
  size_t cluster_left = utils->cluster_size;

  // buscar el primer cluster
  fseek(file, data_start + cluster_size * (cluster - 2), SEEK_SET);

  // leer el archvo hasta que se termine o encontremos el cluster 0xFFFF
  while(file_left > 0 && cluster != 0xFFFF) {
    bytes_to_read = sizeof(buffer);

    // no pasarse del limite
    if(bytes_to_read > file_left) {
      bytes_to_read = file_left;
    }

    if(bytes_to_read > cluster_left) {
      bytes_to_read = cluster_left;
    }

    bytes_read = fread(buffer, 1, bytes_to_read, file);
    res << buffer;

    cluster_left -= bytes_read;
    file_left -= bytes_read;

    // obtener el siguiente cluster de la FAT
    if(cluster_left == 0) {
      fseek(file, fat_start + cluster * 2, SEEK_SET);
      fread(&cluster, 2, 1, file);

      fseek(file, data_start + cluster_size * (cluster - 2), SEEK_SET);
      cluster_left = cluster_size;
    }
  }

  return res.str();
}
