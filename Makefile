TARGET = main
BINARY = bin

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

$(TARGET) : $(OBJ)
	gcc -lreadline $(OBJ) -o $(TARGET) -lm -g

%.o : %.c
	gcc -c $< -o $@ -g

clean :
	rm $(TARGET) *.o

gdb :
	gcc -lreadline $(SRC) -o $(BINARY) -g

gdb_run :
	gdb -tui ./bin

gen :
	dot -Tpng tree.gv -o tree.png