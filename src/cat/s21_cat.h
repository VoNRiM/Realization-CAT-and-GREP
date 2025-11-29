#ifndef S21_CAT_H
#define S21_CAT_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int print_file(char *name, char *flags, int *index);
void flags_parser(char *flags, int argc, char **argv, int *index);
void append_flags(char *flags, char flag);
void append_flag(char *flags, char flag);
void print_symb(int c, int *prev, char *flags, int *index, int *eline_printed);
void parser_gnu_flag(char *flags, char *gnu_flag);

#endif