#include <stdio.h>
#include <unistd.h>

int height = 20;
int width = 20;
int sx = 10;
int sy = 10;
char cmd = 'l';

void draw() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (j == sx && i == sy) {
				fputc('0', stdout);
			} else if (i == 0 || i == height-1 || j == 0 || j == width-1) {
				fputc('#', stdout);
			} else {
				fputc(' ', stdout);
			}
		}
		fputc('\n', stdout);
	}
}

void move() {
	char in = fgetc(stdin);
	if (in == 'h' || in == 'j' || in == 'k' || in == 'l') {
		cmd = in;
	}
	switch (cmd) {
	case 'h':
		sx--;
		break;
	case 'j':
		sy++;
		break;
	case 'k':
		sy--;
		break;
	case 'l':
		sx++;
		break;
	}
}

int logic() {
	if (sx == 0 || sx == width-1 || sy == 0 || sy == height-1) {
		return 0;
	}
	return 1;
}

int main() {
	while (logic()) {
		draw();
		move();
	}

	printf("Game Over!\n");

	return 1;
}
