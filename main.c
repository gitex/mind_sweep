#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include "console.h"

#define FILENAME                  "steps.md"

#define H_DELIMITER_SIGN          '-'
#define H_DELIMITER_WIDTH         32
#define FIRST_ROW                 1
#define CONSOLE_HELP_LINE         1
#define OUTPUT_ROW                3
#define PROGRESS_ROW              5
#define INPUT_ROW                 7

#define OUTPUT_MAX_SIZE           256
#define STEPS_MAX_SIZE            1024


typedef struct Step {
    char name[64];
    int level;
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

    strcat(buff, ANSI_COLOR_GREEN);
    strcat(buff, step->name);
    strcat(buff, ANSI_COLOR_RESET);
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

    Step *steps = (Step*)malloc(STEPS_MAX_SIZE * sizeof(Step));
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
    clear_screen();
    int on = 1;

    char *commands[2][2] = {
        {"n", "next"},
        {"q", "exit"},
    };
    size_t commands_size = sizeof(commands) / sizeof(*commands);
    Step *curr_step = NULL;
    char output[OUTPUT_MAX_SIZE];
    int progress = 1;

    while(on) {
        move_to_row(CONSOLE_HELP_LINE);
        printf("Help: ");
        for (int i = 0; i < commands_size; i++) {
            print_colored(commands[i][0], ANSI_COLOR_GREEN);
            printf(" - %s", commands[i][1]);
            if (i != commands_size - 1) { printf(", "); }
        }
        draw_hline(2, H_DELIMITER_WIDTH, H_DELIMITER_SIGN);
        draw_hline(4, H_DELIMITER_WIDTH, H_DELIMITER_SIGN);
        draw_hline(6, H_DELIMITER_WIDTH, H_DELIMITER_SIGN);
        move_to_row(INPUT_ROW);

        char c = getchar();

        switch (c) {
            case 'q': on = 0; break;
            case 'n':
                clear_row(INPUT_ROW);
                clear_row(OUTPUT_ROW);
                curr_step = find_next_step(steps, curr_step);
                fill_output(output, curr_step);
                print_at_row(OUTPUT_ROW, output);
                break;
        }
        fflush(stdout);
    }
    clear_screen();
    move_to_row(FIRST_ROW);
    free(steps);
    return 0;
}
