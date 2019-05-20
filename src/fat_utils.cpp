#include <sstream>
#include "fat_utils.h"

using std::stringstream;

vector<string> split_path(string path) {
  // recibe un path de directorios y lo divide en un vector:
  // "/SUBDIR/TEST" => ["SUBDIR", "TEST"]
  vector<string> subdirs;
  stringstream ss(path.substr(1, path.size() - 1));
  string buffer;

  while(getline(ss, buffer, '/')) {
    subdirs.push_back(buffer);
  }

  return subdirs;
}
