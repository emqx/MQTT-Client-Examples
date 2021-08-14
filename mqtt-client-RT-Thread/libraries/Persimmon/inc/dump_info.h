#ifndef __DUMP_INFO_H
#define __DUMP_INFO_H

#include <string>

#define DUMP_MODE_HEX   1
#define DUMP_MODE_SORT  2

void prc_dump_info(const std::string &path, int mode);

#endif
