CC = gcc
CFLAGS = -g  -fno-omit-frame-pointer -O0 -Wall -Werror -Wextra -Iinclude
LDFLAGS =

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
HEADER = $(wildcard include/*.h)

TARGET = build/virens

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS)$(OBJ) -o $@

build/%.o: src/%.c $(HEADER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/*

-include $(DEP)
