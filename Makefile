CC=g++
FLAGS=-O5

LIBOPTIONS=-L. -lfreeimageplus

SRC=src/*.cc

BIN=cmkv

cmkv:
	$(CC) $(FLAGS) $(SRC) $(LIBOPTIONS) -o $(BIN)

clean:
	$(RM) $(BIN)
