CC=gcc
CFLAGS=-I.

STORAGE=QUAD

COMMUNICATION=SHARED_MEM

GAME_TYPE=TWO_SHELL

MAIN_FLAGS= -D $(GAME_TYPE) -D $(COMMUNICATION) -D $(STORAGE)

LIBS=-lm -lpthread -lrt

out: main.o storage.o structures.o graphics.o communications.o
	$(CC) *.o -o battleship $(LIBS)

main.o:
	$(CC) $(MAIN_FLAGS) -c main.c gameplay.c $(LIBS)

graphics.o:
	$(CC) $(MAIN_FLAGS) -c ./graphics/graphics.c ./graphics/shell.c $(LIBS)

structures.o:
	$(CC) $(MAIN_FLAGS) -c ./structures/pieces.c ./structures/position.c ./structures/game.c ./structures/gamepieces.c ./structures/gamestructures.c $(LIBS)

storage.o:
ifeq ($(STORAGE), MATRIX)
	$(CC) $(MAIN_FLAGS) -c ./storagestructures/bitmap.c ./storagestructures/linkedlist.c ./storagestructures/matrix.c $(LIBS)
else
	$(CC) $(MAIN_FLAGS) -c ./storagestructures/bitmap.c ./storagestructures/linkedlist.c ./storagestructures/quadtree.c $(LIBS)
endif

communications.o:
ifeq ($(COMMUNICATION), TEXT)
	$(CC) $(MAIN_FLAGS) -c ./communication/communications.c ./communication/text.c $(LIBS)
else
ifeq ($(COMMUNICATION), SHARED_MEM)
	$(CC) $(MAIN_FLAGS) -c ./communication/communications.c ./communication/buffered_communication.c ./communication/sem.c ./communication/sharedmem.c $(LIBS)
else
ifeq ($(COMMUNICATION), NETWORK)
	$(CC) $(MAIN_FLAGS) -c ./communication/communications.c ./communication/buffered_communication.c ./communication/sockets.c $(LIBS)
else
	$(CC) $(MAIN_FLAGS) -c ./communication/communications.c ./communication/same_shell.c $(LIBS)
endif
endif
endif

clean:
	rm *.o battleship