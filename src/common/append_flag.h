#ifndef APPEND_FLAG_H
#define APPEND_FLAG_H

#include <stdlib.h>
#include <string.h>

void append_flag(char *flags, char flag);

struct s_avi_flags {
  char flag;
  char *equivalent_flags;
};

#endif
