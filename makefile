
SRC_DIR := src/
DST_DIR := dist/

MACOSX_TARGET := $(addprefix $(DST_DIR), textgame_MacOSX)

$(MACOSX_TARGET) : $(addprefix $(SRC_DIR), *.c)
	gcc -o $(MACOSX_TARGET) $<

clean:
	rm -rf $(addprefix $(DST_DIR), *)

build: clean $(MACOSX_TARGET)
