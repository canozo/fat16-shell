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

string exit_path(vector<string> path) {
  // recibe un path de directorios y le quita el ultimo directorio:
  // ["SUBDIR, "TEST"] => "/SUBDIR"
  stringstream res("");
  res << "/";
  for (int i = 0; i < path.size() - 1; i += 1) {
    if (i > 0) {
      res << "/";
    }
    res << path[i];
  }

  return res.str();
}
