#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include "part_table.h"
#include "boot_sector.h"
#include "dir_entry.h"
#include "ls.h"
#include "cat.h"
#include "fat_utils.h"
#include "cd.h"
#include "mkdir.h"
#include "cat2.h"

using std::cin;
using std::cout;
using std::stringstream;
using std::vector;
using std::string;

void shell(FILE *, part_table_t *, boot_sector_t *, fat_utils_t *);
vector<string> split(string);
bool validate(vector<string>);
string write_cat();

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
  file = fopen(filename, "rb+");
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
  // calcular offsets y constantes
  fat_utils_t utils;
  utils.fat_start = ftell(file) + (bs.reserved_sectors-1) * bs.sector_size;
  utils.root_start = utils.fat_start + bs.fat_size_sectors * bs.number_of_fats * bs.sector_size;
  utils.data_start = utils.root_start + bs.root_dir_entries * sizeof(dir_entry_t);
  utils.cluster_size = bs.sector_size * bs.sectors_per_cluster;

  shell(file, &fat_pt, &bs, &utils);

  fclose(file);
  return 0;
}

void shell(FILE *file, part_table_t *fat_pt, boot_sector_t *bs, fat_utils_t *utils) {
  string command;
  vector<string> commands;

  string current_dir = "/";
  string new_dir;

  while (true) {
    cout << current_dir << ": $ ";
    getline(cin, command);
    commands = split(command);

    // salir si ingreso "exit"
    if (commands[0] == "exit") {
      break;
    }

    // validar sintaxis correcta y ejecutar
    if (validate(commands)) {
      if (commands[0] == "ls") {
        // ls -l
        // imprime todos los metadatos de los archivos en el directorio actual
        cout << ls(file, fat_pt, bs, utils, current_dir);

      } else if (commands[0] == "cat") {
        if (commands.size() == 2) {
          // cat archivo
          // imprime los datos del archivo de texto
          cout << cat(file, fat_pt, bs, utils, current_dir, commands[1]);

        } else {
          // cat > archivo.txt
          // crea el archivo a.txt en el directorio actual
          cout << cat2(file, fat_pt, bs, utils, current_dir, commands[2], write_cat());

        }
      } else if (commands[0] == "mkdir") {
        // mkdir DIR
        // crea el directorio DIR en el directorio actual
        if (commands[1] == "." || commands[1] == "..") {
          cout << "Nombre de directorio [" << commands[1] << "] reservado.\n\n";
          continue;
        }

        cout << mkdir(file, fat_pt, bs, utils, current_dir, commands[1]);

      } else if (commands[0] == "cd") {
        // cd DIR
        // se mueve al directorio DIR y este es el directorio actual
        if (commands[1] == ".") {
          continue;
        }

        new_dir = cd(file, fat_pt, bs, utils, current_dir, commands[1]);

        if (current_dir == new_dir) {
          cout << "No se encontro el directorio [" << commands[1] << "] en el directorio [" << current_dir << "].\n\n";
        } else {
          current_dir = new_dir;
        }

      } else {
        // ???
        cout << "??? inesperado\n";
      }
    }
  }
}

vector<string> split(string command) {
  string buffer;
  stringstream ss(command);
  vector<string> res;

  while (ss >> buffer) {
    res.push_back(buffer);
  }

  return res;
}

bool validate(vector<string> commands) {
  // verificar que ingreso un comando valido
  if (commands.size() < 2) {
    cout << "Ingreso muy pocos argumentos.\n";
    return false;
  }

  if (commands.size() > 3) {
    cout << "Ingreso muchos argumentos.\n";
    return false;
  }

  // verificar primer comando
  if (commands[0] != "ls" && commands[0] != "cat" && commands[0] != "mkdir" && commands[0] != "cd") {
    cout << "Argumento \"" << commands[0] << "\" no valido.\n";
    return false;
  }

  // ls -l
  if (commands[0] == "ls" && commands[1] != "-l") {
    cout << "Argumento \"" << commands[1] << "\" no valido.\n";
    return false;
  }

  // cat > archivo.txt
  if (commands.size() == 3) {
    if (commands[0] != "cat") {
      cout << "Muchos argumentos para \"" << commands[0] << "\".\n";
      return false;
    }

    if (commands[1] != ">") {
      cout << "Argumento \"" << commands[1] << "\" no valido.\n";
      return false;
    }
  }

  // sintaxis correcta
  return true;
}

string write_cat() {
  stringstream ss("");

  string linea;
  while (getline(cin, linea)) {
    if (linea == ":q") {
      break;
    }
    ss << linea << '\n';
  }

  return ss.str();
}
