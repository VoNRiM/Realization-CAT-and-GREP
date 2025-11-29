CC = gcc
FLAGS = -std=c11 -Wall -Wextra -Werror 
TARGET = s21_grep
SOURCES = s21_grep.c

$(TARGET):
	$(CC) $(FLAGS) -o $(TARGET) $(SOURCES)

all: $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all