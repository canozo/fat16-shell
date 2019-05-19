#ifndef CAT_H
#define CAT_H

#include <string>
#include "part_table.h"
#include "boot_sector.h"
#include "fat_utils.h"

using std::string;

string cat(FILE *, part_table_t *, boot_sector_t *, fat_utils_t *, string, string);

#endif
