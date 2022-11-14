C_FLAGS=-g -Wall
APP=topological

make:
	gcc -g -Wall *.c -o $(APP)

run:
	./$(APP)

valgrind:
	valgrind ./$(APP)

clean:
	rm -f $(APP) stats.txt
