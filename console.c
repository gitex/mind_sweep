#include <stdio.h>
#include "console.h"


void move_to(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

void move_to_row(int row) {
    move_to(row, 1);
}

void print_colored(const char *text, const char *color) {
    printf("%s%s%s", color, text, ANSI_COLOR_RESET);
}

void clear_screen(void) {
    printf(ANSI_CLEAR_SCREEN);
}

void print_at_row(int row, const char *text) {
    move_to_row(row);
    printf(" %s", text);
}

void clear_row(int row) {
    move_to_row(row);
    printf(ANSI_CLEAR_CURRENT_ROW);
}

void draw_hline(int row, int width, char ch) {
    move_to_row(row);
    for (int i = 0; i < width; i++) {
        putchar(ch);
    }
    putchar('\n');
}

