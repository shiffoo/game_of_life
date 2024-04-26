#include <ncurses.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25

void read_initial_state(char field[HEIGHT][WIDTH]);
void draw_field(char field[HEIGHT][WIDTH]);
void copy_field(char src[HEIGHT][WIDTH], char dest[HEIGHT][WIDTH]);
int any_life_around(char field[HEIGHT][WIDTH], int x, int y);
void next_gen(char field[HEIGHT][WIDTH], char changed_field[HEIGHT][WIDTH]);

int main() {
    char field[HEIGHT][WIDTH] = {0};
    read_initial_state(field);
    if (freopen("/dev/tty", "r", stdin) != NULL) {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);

        char changed_field[HEIGHT][WIDTH] = {0};
        copy_field(field, changed_field);

        int speed = 100;
        int gen = 0;

        while (1) {
            int ch = getch();
            if (ch != ERR) {
                switch (ch) {
                    case 'q':
                        endwin();
                        return 0;
                    case '+':
                        if (speed > 10) speed -= 10;
                        break;
                    case '-':
                        if (speed < 1000) speed += 10;
                        break;
                }
            }

            clear();
            draw_field(changed_field);
            mvprintw(
                HEIGHT + 1, 0,
                "Speed: %d ms | Gen: %d | Press 'q' to quit, '+' to increase speed, '-' to decrease speed",
                speed, gen);
            refresh();

            next_gen(field, changed_field);
            copy_field(changed_field, field);

            gen++;
            usleep(speed * 1000);
        }

        endwin();
    }
    return 0;
}

void read_initial_state(char field[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (scanf("%c", &field[i][j]) != 1) {
                for (int k = i; k < HEIGHT; k++) {
                    for (int l = (k == i) ? j : 0; l < WIDTH; l++) {
                        field[k][l] = '-';
                    }
                }
                return;
            }
            if (field[i][j] == '\n' || field[i][j] == '\r') {
                j--;
            }
        }
    }
}

void draw_field(char field[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvaddch(i, j, field[i][j]);
        }
    }
}

void copy_field(char src[HEIGHT][WIDTH], char dest[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

int any_life_around(char field[HEIGHT][WIDTH], int x, int y) {
    int life = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int newX = (x + i + HEIGHT) % HEIGHT;
            int newY = (y + j + WIDTH) % WIDTH;
            if (field[newX][newY] == 'o') life++;
        }
    }
    return life;
}

void next_gen(char field[HEIGHT][WIDTH], char changed_field[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int life = any_life_around(field, i, j);
            changed_field[i][j] = field[i][j] == 'o' ? (life == 2 || life == 3) ? 'o' : '-'
                                  : (life == 3)      ? 'o'
                                                     : '-';
        }
    }
}
