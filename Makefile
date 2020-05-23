CC=gcc
CFLAGS=-I.

STORAGE=QUAD

COMMUNICATION=SAME_SHELL

GS_FLAG=-D $(STORAGE)

CM_FLAGS=-D $(COMMUNICATION)

LIBS=-lm -lpthread -lrt

out: main.o storage.o structures.o graphics.o communications.o
	$(CC) -o battleship

main.o:
	$(CC) -c main.c gameplay.c

graphics.o:
	$(CC) -c ./graphics/graphics.c ./graphics/shell.c

structures.o:
	$(CC) $(GS_FLAG) -c ./structures/pieces.c ./structures/position.c ./structures/game.c ./structures/gamepieces.c ./structures/gamestructures.c

storage.o:
ifeq ($(STORAGE), MATRIX)
	$(CC) -c ./storagestructures/bitmap.c ./storagestructures/linkedlist.c ./storagestructures/matrix.c
else
	$(CC) -c ./storagestructures/bitmap.c ./storagestructures/linkedlist.c ./storagestructures/quadtree.c
endif

communications.o:
ifeq ($(COMMUNICATION), TEXT)
	$(CC) $(CM_FLAGS) -c ./communication/communications.c ./communication/text.c
else
ifeq ($(COMMUNICATION), PIPES)
	$(CC) $(CM_FLAGS) -c ./communication/communications.c #TODO
else
	$(CC) $(CM_FLAGS) -c ./communication/communications.c ./communication/same_shell.c
endif
endif