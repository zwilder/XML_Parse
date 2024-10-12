PROJ_NAME = XML_Parse
SRC_DIR = ./src
OBJ_DIR = ./objs
INC_DIR = ./include
CC = gcc
CFLAGS = -I$(INC_DIR)/ 
LDFLAGS = -lm
OFLAGS = -O2
GFLAGS = -g -Wall -D_POSIX_C_SOURCE=200809L
DEPS = $(OBJECTS:.o=.d)

ifeq (,$(wildcard $(OBJ_DIR)))
	_ := $(shell mkdir -p $(OBJ_DIR))
endif

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

.PHONY: all clean dev

all: $(PROJ_NAME)

$(PROJ_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(OFLAGS)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(GFLAGS) -MMD -MP -c $< -o $@

clean:
	rm $(OBJECTS) $(DEPS) $(PROJ_NAME)

-include $(DEPS)

dev: $(PROJ_NAME)
	ctags -R *
