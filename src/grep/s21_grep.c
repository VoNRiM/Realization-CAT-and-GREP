#include "s21_grep.h"

#include "../common/append_flag.h"

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: %s [flags] word file1 [file2 ...]\n", argv[0]);
    printf("Or: %s [flags] -e pattern [-e pattern2 ...] file1 [file2 ...]\n",
           argv[0]);
    return 1;
  }
  // char flags[10] = "\0";
  int index_end_flags = 0;
  int files_count = 0;  // Создаём переменную в которой будет храниться кол-во
                        // файлов, для отображения.
  t_flags flags_named = {0};
  flags_parser(&flags_named, argc, argv, &index_end_flags);

  int total_patterns_args =
      (flags_named.pattern_count + flags_named.pattern_files_count) * 2;
  index_end_flags += total_patterns_args;

  if (flags_named.pattern_count == 0 && flags_named.pattern_files_count == 0 &&
      index_end_flags + 1 < argc) {
    // if (strlen(argv[index_end_flags +1]) == 0){
    //   return 2;;
    // }
    flags_named.patterns[0] = (char *)argv[index_end_flags + 1];
    flags_named.pattern_count = 1;
    index_end_flags++;
  }
  flag_check(&flags_named, flags_named.flags_buffer);

  if (flags_named
          .patterns_from_file) {  // считываем паттерны в файле если такие есть
    // printf("DEBUG: count patterns from files:
    // %d\n",flags_named.pattern_files_count);
    for (int j = 0; j < flags_named.pattern_files_count; j++) {
      load_patterns_file(&flags_named, flags_named.patterns_files[j]);
    }
  }
  if (!compile_patterns(&flags_named)) {
    return 2;  // error
  }
  for (int j = index_end_flags + 1; j < argc;
       j++) {  // считаем кол-во файлов для вывода в отдельную переменную
    {
      files_count++;
    }
  }
  for (int i = index_end_flags + 1; i < argc; i++) {
    const char *filename = argv[i];
    // printf("DEBUG: work with file: %s\n",filename);
    search_word(&flags_named, filename, &files_count);
  }
  free_compiled_patterns(&flags_named);
  return 0;
}

static FILE *open_search_file(const char *name, const t_flags *flags,
                              int *is_stdin) {
  *is_stdin = 0;
  FILE *result = NULL;
  if (strcmp(name, "-") == 0) {
    *is_stdin = 1;
    result = stdin;
  } else {
    result = fopen(name, "r");
  }
  if (result == NULL) {
    if (flags->suppress_errors == 0) {
      printf("grep: %s: No such file or directory\n", name);
    }
  }
  return result;
}

static void process_single_line(char *line, const t_flags *flags,
                                const char *filename, int files_count,
                                int line_number, int *found_count) {
  int found = 0;
  int match_count = 0;  //

  if (flags->only_mattching && flags->invert_match) {
    return;
  }
  if (flags->only_mattching) {
    match_count =
        regex_search_and_print(line, flags, filename, files_count, line_number);

    if (match_count > 0) {
      if (!flags->count_only) {
        *(found_count) += match_count;
      } else {
        *(found_count) += 1;
      }
    }
    return;
  } else {
    found = regex_search(line, flags);
  }
  if (flags->invert_match) {
    found = !found;
  }
  if (found) {
    (*found_count)++;
  }
  if (flags->files_only && found) {
    printf("%s\n", filename);
    return;
  }
  // printf("DEBUG Files_count:%d",files_count);
  if (flags->line_numbers && !flags->only_mattching && found &&
      !flags->count_only) {
    if (flags->files_only != 1 && files_count > 1 && flags->only_text == 0) {
      printf("%s:", filename);
    }
    printf("%d:%s\n", line_number, line);
  }
  if (found && !flags->count_only && !flags->line_numbers &&
      !flags->only_mattching) {
    if (files_count > 1 && !flags->only_text) {
      printf("%s:", filename);
    }
    printf("%s\n", line);
  }
}

static void print_final_results(const t_flags *flags, const char *filename,
                                int files_count, int found_count) {
  if (flags->count_only && !flags->files_only) {
    int all_pattern_empty = 1;
    for (int i = 0; i < flags->pattern_count; i++) {
      if (strlen(flags->patterns[i]) > 0) {
        all_pattern_empty = 0;
        break;
      }
    }
    if (!(flags->invert_match && all_pattern_empty && found_count == 0)) {
      if (files_count > 1 && !flags->only_text) {
        printf("%s:", filename);
      }
      printf("%d\n", found_count);
    }
  }
}

void search_word(const t_flags *flags_named, const char *name,
                 const int *files_count) {
  int line_number = 0;
  int is_stdin = 0;
  int found_has_match = 0;
  FILE *f = open_search_file(name, flags_named, &is_stdin);
  if (f == NULL) return;
  // printf("DEBUG files count = %d\n", *files_count);
  // printf("DEBUG paterns:%s\n",flags_named->patterns[1]);
  char line[1024];
  while ((fgets(line, sizeof(line), f)) != NULL) {
    line_number++;

    line[strcspn(line, "\n")] = '\0';

    process_single_line(line, flags_named, name, *files_count, line_number,
                        &found_has_match);
    if (flags_named->files_only && found_has_match > 0) {
      break;
    }
  }

  print_final_results(flags_named, name, *files_count, found_has_match);
  if (!is_stdin) {
    fclose(f);
  }
}

void flags_parser(t_flags *flags, int argc, char **argv, int *index) {
  flags->pattern_count = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' || strcmp(argv[i], "--") == 0 ||
        strcmp(argv[i], "-") == 0) {
      break;
    }
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
      if (flags->pattern_count < MAX_PATTERNS) {
        // if (strlen(argv[i+1])==0){
        //   exit(2);
        // }
        flags->patterns[flags->pattern_count] = argv[i + 1];
        flags->pattern_count++;
        // printf("DEBUG: added pattern '%s'\n", argv[i + 1]);
        append_flags(flags, 'e');
      }
      i++;
      continue;
    }
    if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
      if (flags->pattern_files_count < MAX_PATTERNS) {
        flags->patterns_files[flags->pattern_files_count] = argv[i + 1];
        flags->pattern_files_count++;
        // printf("DEBUG:added pattern file '%s'\n", argv[i + 1]);
        append_flags(flags, 'f');
      }
      i++;
      continue;
    }

    *index = i;
    for (size_t j = 1; j < strlen(argv[i]); j++) {
      // Сравниваем индекс позиции с длинной строки
      append_flags(flags, argv[i][j]);
    }
  }
}
void append_flags(t_flags *flags, char flag) {
  struct s_avi_flags avi_flags[10] = {
      {'i', "i"}, {'v', "v"}, {'e', "e"}, {'c', "c"}, {'l', "l"},
      {'n', "n"}, {'h', "h"}, {'s', "s"}, {'o', "o"}, {'f', "f"},
  };

  for (int i = 0; i < 10; i++) {
    if (avi_flags[i].flag == flag) {
      for (size_t j = 0; j < strlen(avi_flags[i].equivalent_flags); j++) {
        append_flag(flags->flags_buffer, avi_flags[i].equivalent_flags[j]);
      }
      break;
    }
  }
}
void append_flag(char *flags, char flag) {
  if (strchr(flags, flag) == NULL) {
    size_t len = strlen(flags);
    if (len < 6) {
      flags[len] = flag;
      flags[len + 1] = '\0';
    }
  }
}
void flag_check(t_flags *flags, char *flag_str) {
  for (size_t i = 0; i < strlen(flag_str); i++) {
    switch (flag_str[i]) {
      case 'v':
        flags->invert_match = 1;
        break;
      case 'i':
        flags->ignore_case = 1;
        break;
      case 'c':
        flags->count_only = 1;
        break;
      case 'l':
        flags->files_only = 1;
        break;
      case 'n':
        flags->line_numbers = 1;
        break;
      case 's':
        flags->suppress_errors = 1;
        break;
      case 'h':
        flags->only_text = 1;
        break;
      case 'o':
        flags->only_mattching = 1;
        break;
      case 'f':
        flags->patterns_from_file = 1;
        break;
      case 'e':
        flags->strong_pattern = 1;
        break;
    }
  }
}
char *str_to_lower(const char *str) {
  if (str == NULL) return NULL;
  char *result = my_strdup(str);
  if (result == NULL) return NULL;
  for (int i = 0; result[i]; i++) {
    result[i] = tolower(result[i]);
  }
  return result;
}
int case_insensitive_case(const char *haystack, const char *needle) {
  if (haystack == NULL || needle == NULL) return 0;
  char *haystack_lower = str_to_lower(haystack);
  char *needle_lower = str_to_lower(needle);
  if (haystack_lower == NULL || needle_lower == NULL) {
    free(haystack_lower);
    free(needle_lower);
    return 0;
  }
  int result = (strstr(haystack_lower, needle_lower) != NULL);
  free(haystack_lower);
  free(needle_lower);
  return result;
}

void load_patterns_file(t_flags *flags, const char *filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    if (!flags->suppress_errors) {
      printf("grep: %s: No such file or directory\n", filename);
    }
    return;
  }
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = '\0';
    if (strlen(line) == 0) continue;
    if (flags->pattern_count < MAX_PATTERNS) {
      flags->patterns[flags->pattern_count] = my_strdup(line);
      flags->pattern_count++;
    } else {
      break;
    }
  }
  fclose(f);

  // int all_patterns_empty = 1;
  // for(int i=0; i<flags->pattern_count; i++){
  //   if (strlen(flags->patterns[i])>0){
  //     all_patterns_empty = 0;
  //     break;
  //   }
  // }
  // if (all_patterns_empty){
  //   exit(2);
  // }
}

int compile_patterns(t_flags *flags) {
  int regex_flags = 0;

  if (flags->strong_pattern) {
    regex_flags = REG_EXTENDED;
  }

  if (flags->ignore_case) {
    regex_flags |= REG_ICASE;
  }

  flags->regex_compiled_count = 0;

  for (int i = 0; i < flags->pattern_count; i++) {
    int ret =
        regcomp(&flags->compiled_patterns[i], flags->patterns[i], regex_flags);
    if (ret != 0) {
      // error
      char error_buf[100];
      regerror(ret, &flags->compiled_patterns[i], error_buf, sizeof(error_buf));
      if (!flags->suppress_errors) {
        printf("grep: invalid regex '%s': %s\n", flags->patterns[i], error_buf);
      }

      for (int j = 0; j < flags->regex_compiled_count; j++) {
        regfree(&flags->compiled_patterns[j]);
      }
      return 0;  // error
    }
    flags->regex_compiled_count++;
  }
  return 1;  // well_done
}

int regex_search(const char *line, const t_flags *flags) {
  for (int i = 0; i < flags->regex_compiled_count; i++) {
    int ret = regexec(&flags->compiled_patterns[i], line, 0, NULL, 0);

    if (ret == 0) {
      return 1;  // find
    } else if (ret != REG_NOMATCH) {
      char error_buf[100];
      regerror(ret, &flags->compiled_patterns[i], error_buf, sizeof(error_buf));
      printf("grep: regex execution error: %s\n", error_buf);
    }
  }
  return 0;  // no find
}

int regex_search_and_print(const char *line, const t_flags *flags,
                           const char *filename, int files_count,
                           int line_number) {
  regmatch_t matches[1];
  int match_count = 0;
  for (int i = 0; i < flags->regex_compiled_count; i++) {
    const char *search_pos = line;
    while (regexec(&flags->compiled_patterns[i], search_pos, 1, matches, 0) ==
           0) {
      if (matches[0].rm_so == -1) break;
      int start = matches[0].rm_so;
      int end = matches[0].rm_eo;

      int length = end - start;
      // if (length == 0){
      //   search_pos++;
      //   if (*search_pos == '\0') break;
      // }
      // printf("DEBUG: Found '%.*s' at pos %d-%d (len=%d)\n", length,
      if (!flags->count_only) {
        if (files_count > 1 && !flags->only_text) {
          printf("%s:", filename);
        }
        if (flags->line_numbers) {
          printf("%d:", line_number);
        }
        printf("%.*s\n", length, search_pos + start);
      }
      match_count++;

      if (length == 0) {
        search_pos++;
      } else {
        search_pos += end;
      }
      if (*search_pos == '\0') break;
    }
  }
  return match_count;
}

void free_compiled_patterns(t_flags *flags) {
  for (int i = 0; i < flags->regex_compiled_count; i++) {
    regfree(&flags->compiled_patterns[i]);
  }
  flags->regex_compiled_count = 0;

  for (int i = 0; i < flags->pattern_count; i++) {
    if (flags->patterns_from_file && flags->patterns[i] != NULL) {
      free(flags->patterns[i]);
      flags->patterns[i] = NULL;
    }
  }
}

char *my_strdup(const char *str) {
  if (str == NULL) return NULL;

  size_t len = strlen(str) + 1;
  char *result = malloc(len);
  if (result != NULL) {
    memcpy(result, str, len);
  }
  return result;
}