#include <stdio.h>
#include <stdlib.h>
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

int isSnake(int x, int y) {
	for (int i = 0; i < len; i++) {
		Pos s = *snake[i];
		if (s.x == x && s.y == y) {
			return 1;
		}
	}
	return 0;
}

void draw() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (isSnake(j, i)) {
				fputc('O', stdout);
			} else if (j == apple.x && i == apple.y) {
				fputc('@', stdout);
			} else if (i == 0 || i == height-1 || j == 0 || j == width-1) {
				fputc('#', stdout);
			} else {
				fputc(' ', stdout);
			}
		}
		fputc('\n', stdout);
	}
	printf("Score: %d\n", score);
	printf("Move#: %d\n", moveNum);
}

void move() {
	char in = fgetc(stdin);
	if (in == 'h' || in == 'j' || in == 'k' || in == 'l') {
		cmd = in;
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
	init();
	while (logic()) {
		draw();
		move();
	}

	printf("Game Over!\n");
	cleanup();

	return 1;
}
