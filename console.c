#include <stdio.h>

#define CONSOLE_CLEAR_CURR_LINE   "\033[2K"
#define CONSOLE_CLEAR_ALL         "\033[2J"
#define CONSOLE_COLOR_RED         "\033[31m"
#define CONSOLE_COLOR_GREEN       "\033[32m"
#define CONSOLE_COLOR_MAGENTA     "\033[35m"
#define CONSOLE_COLOR_BLUE        "\033[34m"
#define CONSOLE_COLOR_RESET       "\033[0m"

#define CONSOLE_FIRST_COLUMN      1


void move_to(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

void move_to_row(int row) {
    move_to(row, CONSOLE_FIRST_COLUMN);
}

void print_colored(char *text, char *color) {
    printf("%s%s%s", color, text, CONSOLE_COLOR_RESET);
}

void clear_screen(void) {
    printf(CONSOLE_CLEAR_ALL);
}

void print_at_row(int row, char *text) {
    move_to_row(row);
    printf("%s", text);
}

void clear_row(int row) {
    move_to_row(row);
    printf(CONSOLE_CLEAR_CURR_LINE);
}

void draw_hline(int row, int width, char ch) {
    move_to(row, CONSOLE_FIRST_COLUMN);
    for (int i = 0; i < width; i++) {
        putchar(ch);
    }
    putchar('\n');
}
