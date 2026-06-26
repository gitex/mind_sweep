#ifndef _CONSOLE
#define _CONSOLE

#define ANSI_UPPER_LEFT_CORNER    "\033[H"
#define ANSI_CLEAR_CURRENT_ROW    "\033[2K"
#define ANSI_CLEAR_SCREEN         "\033[2J"
#define ANSI_COLOR_RED            "\033[31m"
#define ANSI_COLOR_GREEN          "\033[32m"
#define ANSI_COLOR_MAGENTA        "\033[35m"
#define ANSI_COLOR_BLUE           "\033[34m"
#define ANSI_COLOR_RESET          "\033[0m"


void clear_screen(void);
void clear_row(int row);
void move_to(int row, int col);
void move_to_row(int row);
void print_colored(const char *text, const char *color);
void print_at_row(int row, const char *text);
void draw_hline(int row, int width, char ch);

#endif
