CC = gcc
FLAGS = -std=c11 -Wall -Werror -Wextra
TARGET = s21_cat
SOURCES = s21_cat.c

$(TARGET):
	$(CC) $(FLAGS) -o $(TARGET) $(SOURCES)

all: $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all