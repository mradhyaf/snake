#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
	int x;
	int y;
} Pos;

int height = 20;
int width = 20;

Pos **snake;
unsigned int cap = 2;
unsigned int len = 1;
unsigned int head = 1;
unsigned int tail = 0;
Pos apple = { -1, -1 };

char cmd = 'l';

unsigned int score = 0;

Pos** alloc_snake(unsigned int cap) {
	Pos** new_snake = malloc(cap * sizeof(Pos*));
	for (unsigned int i = 0; i < cap; i++) {
		new_snake[i] = malloc(sizeof(Pos));
		new_snake[i]->x = -1;
		new_snake[i]->y = -1;
	}
	return new_snake;
}

void free_snake() {
	for (unsigned int i = 0; i < cap; i++) {
		free(snake[i]);
		snake[i] = NULL;
	}
	free(snake);
	snake = NULL;
}

void grow_snake() {
	len++;
	if (len == cap) {
		Pos **new_snake = alloc_snake(2*cap);
		for (unsigned int i = 0; i < len; i++) {
			*new_snake[i] = *snake[(tail + i) % len];
		}
		free_snake();
		snake = new_snake;
		tail = 0;
		head = len - 1;
		cap *= 2;
	}
}

void move_snake() {
	Pos headv = *snake[head];
	if ((head - tail + cap) % cap == len) {
		snake[tail]->x = snake[tail]->y = -1;
		tail = (tail + 1) % cap;
	}
	head = (head + 1) % cap;
	switch (cmd) {
	case 'h':
		headv.x--;
		break;
	case 'j':
		headv.y++;
		break;
	case 'k':
		headv.y--;
		break;
	case 'l':
		headv.x++;
		break;
	}
	*snake[head] = headv;
}

void gen_apple() {
	apple.x = rand() % (width-2) + 1;
	apple.y = rand() % (height-2) + 1;
}

void set_nblocking_io() {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void set_blocking_io() {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

void set_terminal_mode() {
	// Save terminal
	system("tput smcup");
	// Set non-canonical
	struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void reset_terminal_mode() {
	// Reset canonical
	struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	// Restore terminal
	system("tput rmcup");
}

void init() {
	snake = alloc_snake(cap);
	snake[head]->x = 10;
	snake[head]->y = 10;
}

void cleanup() {
	free_snake();
}

int is_snake(int x, int y) {
	for (int i = 0; i < len; i++) {
		Pos *s = snake[(tail + i) % cap];
		if (s->x == x && s->y == y) {
			return 1;
		}
	}
	return 0;
}

void draw() {
	printf("\e[1;1H\e[2J");
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (is_snake(j, i)) {
				putchar('O');
			} else if (j == apple.x && i == apple.y) {
				putchar('@');
			} else if (i == 0 || i == height-1 || j == 0 || j == width-1) {
				putchar('#');
			} else {
				putchar(' ');
			}
		}
		putchar('\n');
	}
	printf("Score: %d\n", score);
}

void input() {
	char in;
	while ((in = getchar()) != EOF) {
		if (in == 'h' || in == 'j' || in == 'k' || in == 'l') {
			cmd = in;
		}
	}
}

int logic() {
	move_snake();

	if (snake[head]->x == 0 || snake[head]->x == width - 1|| snake[head]->y == 0 || snake[head]->y == height - 1) {
		return 0;
	}
	
	if (snake[head]->x == apple.x && snake[head]->y == apple.y) {
		apple.x = apple.y = -1;
		score++;
		grow_snake();
	}

	while (apple.x == -1) {
		gen_apple();
	}
	
	return 1;
}

int main() {
	set_terminal_mode();
	set_nblocking_io();
	init();
	while (logic()) {
		draw();
		usleep(400 * 1000);
		input();
	}
	printf("Game Over!\nPress any key to exit...\n");
	set_blocking_io();
	fgetc(stdin);
	cleanup();
	reset_terminal_mode();
	return 1;
}
