C_FLAGS=-g -Wall
APP=topological
SRC_FILES=$(wildcard *.c)
OUT=$(wildcard stats_*)

make:
	gcc -g -Wall $(SRC_FILES) -o $(APP)

run:
	./$(APP)

valgrind:
	valgrind ./$(APP)

clean:
	rm -f $(APP) $(OUT)
