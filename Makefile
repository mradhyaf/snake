build:
	gcc -Wall -o snake snake.c

debug:
	gcc -Wall -g -o snake snake.c
	gdb ./snake

clean:
	rm -f *.out snake