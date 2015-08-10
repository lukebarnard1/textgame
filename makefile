
SRC_DIR := src/
BLD_DIR := build/
DST_DIR := dist/
INC_DIR := include/

MACOSX_TARGET := $(addprefix $(DST_DIR), textgame_MacOSX)
DEBUG_MACOSX_TARGET := $(addprefix $(DST_DIR), textgame_MacOSX_debug)

$(MACOSX_TARGET) : $(addprefix $(SRC_DIR), *.c)
	gcc -Iinclude -Wall -o $(MACOSX_TARGET) $^

$(DEBUG_MACOSX_TARGET) : $(addprefix $(SRC_DIR), *.c)
	gcc -Iinclude -Wall -DDEBUG_ON -o $(DEBUG_MACOSX_TARGET) $^

clean:
	rm -rf $(addprefix $(DST_DIR), *)

build: clean $(MACOSX_TARGET)

debug_build: $(DEBUG_MACOSX_TARGET)