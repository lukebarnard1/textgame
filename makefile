
SRC_DIR := src/
BLD_DIR := build/
DST_DIR := dist/
INC_DIR := include/

MACOSX_TARGET := $(addprefix $(DST_DIR), textgame_MacOSX)

$(MACOSX_TARGET) : $(addprefix $(SRC_DIR), *.c)
	gcc -Iinclude -o $(MACOSX_TARGET) $^

clean:
	rm -rf $(addprefix $(DST_DIR), *)

build: clean $(MACOSX_TARGET)