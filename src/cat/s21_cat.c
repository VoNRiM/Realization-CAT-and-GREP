#include "s21_cat.h"

#include "../common/append_flag.c"

typedef int bool;

int main(int argc, char **argv) {
  int index = 0;
  char flags[10] = "\0";
  int index_end_flags = 0;
  // int prev_no_newline = 0;
  flags_parser(flags, argc, argv, &index_end_flags);

  if (index_end_flags == argc - 1) {
    print_file("-", flags, &index);
  }

  for (int i = index_end_flags + 1; i < argc; i++) {
    if (strcmp(argv[i], "--") == 0) continue;
    print_file(argv[i], flags, &index);
  }

  return 0;
}

int print_file(char *name, char *flags, int *index) {
  static int prev = '\n';
  int err_code = 0;
  FILE *f;
  if (strcmp("-", name) == 0)
    f = stdin;
  else
    f = fopen(name, "rt");
  if (f == NULL) {
    fprintf(stderr, "cat: %s: Нет такого файла или каталога\n", name);
  } else {
    bool eline_printed = 0;
    int c = fgetc(f);  //, prev = '\n';
    while (c != EOF) {
      print_symb(c, &prev, flags, index, &eline_printed);
      c = fgetc(f);
    }

    if (f != stdin)
      fclose(f);
    else {
      err_code = 1;
    }
  }
  return err_code;
}

void flags_parser(char *flags, int argc, char **argv, int *index) {
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--", 2) == 0 && strlen(argv[i]) > 2) {
      *index = i;
      parser_gnu_flag(flags, argv[i]);
    } else if (argv[i][0] != '-' || strcmp(argv[i], "-") == 0) {
      break;
    } else {
      *index = i;
      for (size_t j = 1; j < strlen(argv[i]); j++) {
        // Сравниваем индекс позиции с длинной строки
        append_flags(flags, argv[i][j]);
      }
    }
  }
}

void parser_gnu_flag(char *flags, char *gnu_flag) {
  struct {
    char *gnu_name;
    char equivalent;
  } gnu_flags[] = {
      {"--number-nonblank", 'b'}, {"--number", 'n'}, {"--squeeze-blank", 's'}

  };

  for (int i = 0; i < 3; i++) {
    if (strcmp(gnu_flag, gnu_flags[i].gnu_name) == 0) {
      append_flags(flags, gnu_flags[i].equivalent);
      return;
    }
  }

  for (int i = 0; i < 3; i++) {
    if (strstr(gnu_flags[i].gnu_name, gnu_flag) == gnu_flags[i].gnu_name) {
      append_flags(flags, gnu_flags[i].equivalent);
      return;
    }
  }
}

void append_flags(char *flags, char flag) {
  struct s_avi_flags avi_flags[8] = {{'b', "b"},  {'E', "E"}, {'e', "Ev"},
                                     {'n', "n"},  {'s', "s"}, {'T', "T"},
                                     {'t', "Tv"}, {'v', "v"}};
  for (int i = 0; i < 8; i++) {
    if (avi_flags[i].flag == flag) {
      for (size_t j = 0; j < strlen(avi_flags[i].equivalent_flags); j++) {
        append_flag(flags, avi_flags[i].equivalent_flags[j]);
      }
      break;
    }
  }
}

void print_symb(int c, int *prev, char *flags, int *index,
                bool *eline_printed) {
  if (!(strchr(flags, 's') != NULL && *prev == '\n' && c == '\n' &&
        *eline_printed)) {
    if (*prev == '\n' && c == '\n')
      *eline_printed = 1;
    else
      *eline_printed = 0;

    if (((strchr(flags, 'n') != NULL && strchr(flags, 'b') == NULL) ||
         (strchr(flags, 'b') != NULL && c != '\n')) &&
        *prev == '\n') {
      *(index) += 1;
      printf("%6d\t", *index);
    }
    if (strchr(flags, 'E') != NULL && c == '\n') {
      printf("$");
    }
    if (strchr(flags, 'T') != NULL && c == '\t') {
      printf("^");
      c = 'I';
    }
    if (strchr(flags, 'v') != NULL && c >= 0 && c <= 31 && c != '\n' &&
        c != '\t') {
      printf("^");
      c = c + 64;
    }
    fputc(c, stdout);
  }
  *prev = c;
}