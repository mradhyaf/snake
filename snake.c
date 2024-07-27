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
unsigned int len = 3;
unsigned int head = 0;
Pos apple = { -1, -1 };

char cmd = 'l';
unsigned int moveNum = 0;

unsigned int score = 0;

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
	snake = malloc(len * sizeof(Pos*));
	for (unsigned int i = 0; i < len; i++) {
		snake[i] = malloc(sizeof(Pos));
		snake[i]->x = -1;
		snake[i]->y = -1;
	}
	snake[head]->x = 10;
	snake[head]->y = 10;
}

void cleanup() {
	for (unsigned int i = 0; i < len; i++) {
		free(snake[i]);
	}
	free(snake);
}

int is_snake(int x, int y) {
	for (int i = 0; i < len; i++) {
		Pos s = *snake[i];
		if (s.x == x && s.y == y) {
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

void move() {
	char in;
	while ((in = getchar()) != EOF) {
		if (in == 'h' || in == 'j' || in == 'k' || in == 'l') {
			cmd = in;
		}
	}
	unsigned int prev = head;
	head = (head + 1) % len;
	*snake[head] = *snake[prev];
	switch (cmd) {
	case 'h':
		snake[head]->x--;
		break;
	case 'j':
		snake[head]->y++;
		break;
	case 'k':
		snake[head]->y--;
		break;
	case 'l':
		snake[head]->x++;
		break;
	}
}

void genApple() {
	apple.x = rand() % (width-2) + 1;
	apple.y = rand() % (height-2) + 1;
}

int logic() {
	if (snake[head]->x == 0 || snake[head]->x == width || snake[head]->y == 0 || snake[head]->y == height) {
		return 0;
	}
	
	if (snake[head]->x == apple.x && snake[head]->y == apple.y) {
		apple.x = apple.y = -1;
		score++;
	}

	while (apple.x == -1 || (apple.x == snake[head]->x && apple.y == snake[head]->y)) {
		genApple();
	}

	moveNum++;
	
	return 1;
}

int main() {
	set_terminal_mode();
	set_nblocking_io();
	init();
	while (logic()) {
		draw();
		usleep(200000);
		move();
	}
	printf("Game Over!\nPress any key to exit...\n");
	set_blocking_io();
	fgetc(stdin);
	cleanup();
	reset_terminal_mode();
	return 1;
}
