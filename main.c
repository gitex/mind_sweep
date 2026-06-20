#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

#define ASCII_SPACE 32
#define STEPS_MAX_AMOUNT 1024
#define FILENAME "steps.md"

#define CONSOLE_CLEAR_CURR_LINE   "\033[2K"
#define CONSOLE_CLEAR_ALL         "\033[2J"
#define CONSOLE_COLOR_RED         "\033[31m"
#define CONSOLE_COLOR_GREEN       "\033[32m"
#define CONSOLE_COLOR_MAGENTA     "\033[35m"
#define CONSOLE_COLOR_BLUE        "\033[34m"
#define CONSOLE_COLOR_RESET       "\033[0m"

#define CONSOLE_DELIMITER         '-'
#define CONSOLE_DELIMITER_WIDTH   32
#define CONSOLE_FIRST_LINE        1
#define CONSOLE_HELP_LINE         1
#define CONSOLE_OUTPUT_LINE       3
#define CONSOLE_INPUT_LINE        5

#define OUTPUT_MAX_SIZE           256


typedef struct Step {
    char name[64];
    int level;
    int order;
    struct Step *parent;
    struct Step *prev;
    struct Step *next;
} Step;

Step *find_next_step(Step *steps, Step *curr_step) {
    // TODO: add check for empty list
    Step *curr = curr_step;

    if (curr == NULL && steps != NULL) {
        curr = &steps[0];
    }

    while(curr->next) {
        if (curr->next->level == curr->level) {
            return curr->next;
        } else {
            curr = curr->next;
            continue;
        }
    }
    return curr;
}

void fill_output(char *buff, Step *step) {
    if (step == NULL) {
        return;
    }

    memset(buff, 0, sizeof(&buff));

    Step *curr = step;

    while (curr->parent) {
        strcat(buff, curr->parent->name);
        strcat(buff, " > ");
        curr = curr->parent;
    }

    strcat(buff, CONSOLE_COLOR_GREEN);
    strcat(buff, step->name);
    strcat(buff, CONSOLE_COLOR_RESET);
}

void console_goto_line(int number) {
    // Go to line 1 -> \033[1;1H
    printf("\033[%d;1H", number);
}

void console_color_text(char *str, char *color) {
    printf("%s%s%s", color, str, CONSOLE_COLOR_RESET);
}

void console_clear(void) {
    printf(CONSOLE_CLEAR_ALL);
}

void console_print_at_line(int number, char *str) {
    console_goto_line(number);
    printf("%s", str);
}

void console_clear_line(int number) {
    console_goto_line(number);
    printf(CONSOLE_CLEAR_CURR_LINE);
}

void console_put_delimiter_at_line(int line, int width) {
    console_goto_line(line);
    for (int i = 0; i < width; i++) {
        putchar(CONSOLE_DELIMITER);
    }
    putchar('\n');
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
        end = strchr(start, '\n');
        if (end == NULL) {
            break;
        }
        Step *curr = &steps[idx];

        // trim
        while(*start) {
            if (*start == '*') {
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

    // --- TUI ---
    console_clear();
    int on = 1;

    char *commands[2][2] = {
        {"n", "next"},
        // {"p", "previous"},
        // {"s", "skip"},
        {"q", "exit"},
    };
    size_t commands_size = sizeof(commands) / sizeof(*commands);
    Step *curr_step = NULL;
    char output[OUTPUT_MAX_SIZE];

    while(on) {
        console_goto_line(CONSOLE_HELP_LINE);
        printf("Help: ");
        for (int i = 0; i < commands_size; i++) {
            console_color_text(commands[i][0], CONSOLE_COLOR_RED);
            printf(" - %s", commands[i][1]);
            if (i != commands_size - 1) { printf(", "); }
        }

        console_put_delimiter_at_line(2, CONSOLE_DELIMITER_WIDTH);
        console_put_delimiter_at_line(4, CONSOLE_DELIMITER_WIDTH);
        console_goto_line(CONSOLE_INPUT_LINE);

        char c = getchar();

        switch (c) {
            case 'q': on = 0; break;
            case 'n':
                console_clear_line(CONSOLE_INPUT_LINE);
                console_clear_line(CONSOLE_OUTPUT_LINE);
                curr_step = find_next_step(steps, curr_step);
                fill_output(output, curr_step);
                console_print_at_line(CONSOLE_OUTPUT_LINE, output);
                break;
        }
        fflush(stdout);
    }
    console_clear();
    console_goto_line(CONSOLE_FIRST_LINE);
    free(steps);
    return 0;
}
