CC = gcc
TARGET = ngrav
CFLAGS= -g -O3 -Isrc -Wall `pkg-config --cflags glfw3 gl` -fopenmp
LFLAGS = -lm `pkg-config --libs glfw3 gl` -ldl -lpthread -lwebp

SRCS = \
	  src/main.c \
	  src/ui.c \
	  src/parts.c \
	  src/mathutil.c \
	  src/bhtree.c \
	  src/file.c \
	  src/glad.c \
	  src/quaternion.c
OBJECTS = $(notdir $(SRCS:.c=.o))

all:	$(TARGET)

-include depends

%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS) $(LFLAGS)


clean:
	rm $(OBJECTS)
	rm $(TARGET)

depend:
	$(CC) $(CFLAGS) $(SRCS) -MM >depends

.PHONY: all clean depend
