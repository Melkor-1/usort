CFLAGS 	+= -std=c17
CFLAGS 	+= -no-pie
CFLAGS 	+= -g3
CFLAGS 	+= -ggdb
CFLAGS 	+= -Wall
CFLAGS 	+= -Wextra
CFLAGS 	+= -Warray-bounds
CFLAGS 	+= -Wconversion
CFLAGS 	+= -Wmissing-braces
CFLAGS 	+= -Wno-parentheses
CFLAGS 	+= -Wno-format-truncation
CFLAGS 	+= -Wpedantic
CFLAGS 	+= -Wstrict-prototypes
CFLAGS 	+= -Wwrite-strings
CFLAGS 	+= -Winline
CFLAGS 	+= -s
CFLAGS 	+= -O2
CFLAGS 	+= -D_FORTIFY_SOURCE=2

TARGET	= usort
all: $(TARGET)

clean:
	$(RM) $(TARGET) 

.PHONY: all clean 
.DELETE_ON_ERROR:
