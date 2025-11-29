#include "append_flag.h"

void append_flag(char *flags, char flag) {
  if (strchr(flags, flag) == NULL) {
    size_t len = strlen(flags);
    if (len < 6) {
      flags[len] = flag;
      flags[len + 1] = '\0';
    }
  }
}