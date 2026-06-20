#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

#define ASCII_LINE_FEED 10
#define ASCII_ASTERISK 42
#define ASCII_SPACE 32
#define STEPS_MAX_AMOUNT 1024
#define FILENAME "steps.md"

#define CONSOLE_COLOR_RED         "\033[31m"
#define CONSOLE_COLOR_GREEN       "\033[32m"
#define CONSOLE_COLOR_RESET       "\033[0m"
#define CONSOLE_DELIMITER         '-'

typedef struct Step {
    char name[64];
    int level;
    int order;
    struct Step *parent;
    struct Step *prev;
    struct Step *next;
} Step;

void console_goto_line(int number) {
    // Go to line 1 -> \033[1;1H
    printf("\033[%d;1H", number);
}

void console_color_text(char *str, char *color) {
    printf("%s%s%s", color, str, CONSOLE_COLOR_RESET);
}

void console_clear(void) {
    printf("\033[2J");
}

void console_print_on_line(int number, char *str) {
    console_goto_line(number);
    printf(str);
}

void console_clear_line(int number) {
    console_goto_line(number);
    printf("\033[2K");   // Clear current line
}

void console_divide_by_delimiter(int len) {
    for (int i = 0; i < len; i++) {
        putchar(CONSOLE_DELIMITER);
    }

}

int main(void) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Cannot find a file %s", FILENAME);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    if (size == 0) {
        printf("File is empty");
        return 1;
    }

    char *buff = (char*)malloc(size + 1);
    if (!buff) {
        printf("Cannot allocate memory");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(buff, 1, size, file);
    if (bytes_read != size) {
        printf("Cannot read a file");
        free(buff);
        fclose(file);
        return 1;
    }

    fclose(file);

    char *start = buff;
    char *end;
    int line_n = 0;

    Step *steps = (Step*)malloc(STEPS_MAX_AMOUNT * sizeof(Step));
    if (!steps) {
        printf("Cannot allocate the memory");
        free(buff);
    }

    int idx = 0;
    int order[10] = {0};
    Step *prev = NULL;
    while(*start) {
        end = strchr(start, ASCII_LINE_FEED);
        if (end == NULL) {
            break;
        }
        Step *curr = &steps[idx];

        // trim
        while(*start) {
            if (*start == ASCII_ASTERISK) {
                curr -> level++;
                start++;
            } else if (isspace(*start)) {
                start++;
            } else { break; }
        }

        int len = end - start;
        snprintf(curr->name, sizeof(curr->name), "%.*s", len, start);

        // set order
        order[curr->level]++;
        if (prev && prev->level < curr->level) {
            order[curr->level] = 0;
        }
        curr->order = order[curr->level];

        // parent
        if (prev && prev->level == curr->level) {
            curr->parent = prev->parent;
        } else if (prev && prev->level < curr->level) {
            curr->parent = prev;
        }

        // next
        if (prev) {
            prev->next = curr;
        }

        start = end + 1;
        idx++;
        prev = curr;
    }
    free(buff);

    char parents[256] = "";
    int steps_size = idx;

    // --- Console output ---
    console_clear();
    int on = 1;

    char *cmd[3][2] = {
        {"n", "next"},
        {"p", "previous"},
        {"q", "exit"},
    };
    size_t cmd_size = sizeof(cmd) / sizeof(*cmd);

    while(on) {
        console_goto_line(1);

        for (int i = 0; i < cmd_size; i++) {
            console_color_text(cmd[i][0], CONSOLE_COLOR_GREEN);
            printf(" - %s", cmd[i][1]);
            if (i != cmd_size - 1) { printf(", "); }
        }
        console_goto_line(2);
        printf("-------\n");
        console_goto_line(4);
        printf("-------\n");
        console_goto_line(5);

        char c = getchar();
        switch (c) {
            case 'q': on = 0; break;
            case 'n':
                console_print_on_line(3, "next");
                break;
        }
        fflush(stdout);
    }
    console_clear();
    console_goto_line(1);
    free(steps);
    return 0;
}
