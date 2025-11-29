#ifndef GREP_H
#define GREP_H

#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_PATTERNS 10
#define MAX_FILES 10

typedef struct {
  int invert_match;        // -v
  int ignore_case;         // -i
  int count_only;          // -c
  int files_only;          // -l
  int line_numbers;        // -n
  int suppress_errors;     // -s
  int only_text;           // -h
  int only_mattching;      // -o
  int patterns_from_file;  // -f
  int strong_pattern;      // -e
  int use_regex;           // -для regex
  regex_t compiled_patterns[MAX_PATTERNS];
  int regex_compiled_count;
  // массив шаблонов
  int pattern_count;
  int pattern_files_count;
  char *patterns[MAX_PATTERNS];
  char *patterns_files[MAX_FILES];
  // буфер для строки флагов
  char flags_buffer[10];
} t_flags;

void search_word(const t_flags *flags, const char *name_file,
                 const int *files_count);
void flags_parser(t_flags *flags, int argc, char **argv, int *index);
void append_flags(t_flags *flags, char flag);
void flag_check(t_flags *flags, char *flag_str);
char *str_to_lower(const char *str);
int file_exists(const char *filename);
void load_patterns_file(t_flags *flags, const char *filename);
int regex_search(const char *line, const t_flags *flags);
int regex_search_and_print(const char *line, const t_flags *flags,
                           const char *filename, int files_count,
                           int line_number);
void free_compiled_patterns(t_flags *flags);
int compile_patterns(t_flags *flags);
char *my_strdup(const char *str);

#endif