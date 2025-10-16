CC = gcc -std=c99
CFLAGS = -Wall -Wextra -g

SOURCES = main.c editor.c

OBJECTS = main.o editor.o

OUT = vite

$(OUT): $(OBJECTS)
	$(CC) -o $(OUT) $(OBJECTS) $(LIBS)

main.o: main.c editor.h
	$(CC) -c main.c

editor.o: editor.c editor.h
	$(CC) -c editor.c

-include dependencies.mk

clean:
	del $(OUT) $(OBJECTS)