#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <stdbool.h>
#include "console.h"

#define FILENAME                  "steps.md"

#define H_DELIMITER_SIGN          '-'
#define H_DELIMITER_WIDTH         64
#define FIRST_ROW                 1
#define CONSOLE_HELP_LINE         1
#define OUTPUT_ROW                5
#define PROGRESS_ROW              2
#define INPUT_ROW                 7

#define OUTPUT_MAX_SIZE           256
#define STEPS_MAX_SIZE            1024


typedef struct Step {
    char name[64];
    int level;
    bool is_trigger;
    struct Step *parent;
    struct Step *prev;
    struct Step *next;
} Step;


typedef struct Buffer {
    char *data;
    size_t size;
} Buffer;


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

void clear_buffer(Buffer *buff) {
    memset(buff->data, 0, buff->size);
}

void write_progress(Buffer *buff, int current, int total) {
    int persent = current * 100 / total;

    strcat(buff->data, "progress: [");

    for (int i = 0; i < persent / 2; i++) {
        strcat(buff->data, "#");
    }

    for (int i = 0; i < 50 - persent / 2; i++) {
        strcat(buff->data, ".");
    }

    char temp[64];
    snprintf(temp, sizeof(temp), "] %d/%d", current, total);
    strcat(buff->data, temp);
}

void write_step(Buffer *buff, Step *step) {
    if (step == NULL) {
        return;
    }

    Step *curr = step;

    while (curr->parent) {
        strcat(buff->data, curr->parent->name);
        strcat(buff->data, " > ");
        curr = curr->parent;
    }

    strcat(buff->data, ANSI_COLOR_GREEN);
    strcat(buff->data, step->name);
    strcat(buff->data, ANSI_COLOR_RESET);
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
    int triggers_total = 0;
    while(*start) {
        end = strchr(start, '\n');
        if (end == NULL) {
            break;
        }
        Step *curr = &steps[idx];

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

        // next and previous
        if (prev) {
            prev->next = curr;
            curr->prev = prev;
        }

        // is_trigger (prev)
        if (prev  && prev->next->level >= prev->level) {
            prev->is_trigger = true;
            triggers_total++;
        }

        start = end + 1;
        idx++;
        prev = curr;
    }
    free(buff);

    // --- TUI ---
    clear_screen();
    int quit = 0;

    char *commands[2][2] = {
        {"n", "next"},
        {"q", "exit"},
    };
    size_t commands_size = sizeof(commands) / sizeof(*commands);
    Step *curr_step = NULL;
    int progress_current = 0;
    Buffer progress_buff = {
        .data = (char[128]){0},
        .size = 128
    };

    Buffer output_buff = {
        .data = (char[OUTPUT_MAX_SIZE]){0},
        .size = OUTPUT_MAX_SIZE
    };

    while(!quit) {
        move_to_row(CONSOLE_HELP_LINE);
        printf("help: ");
        for (int i = 0; i < commands_size; i++) {
            print_colored(commands[i][0], ANSI_COLOR_RED);
            printf(" - %s", commands[i][1]);
            if (i != commands_size - 1) { printf(", "); }
        }

        clear_buffer(&progress_buff);
        write_progress(&progress_buff, progress_current, triggers_total);
        print_at_row(PROGRESS_ROW, progress_buff.data);
        move_to_row(INPUT_ROW);

        char c = getchar();

        switch (c) {
            case 'q': quit = 1; break;
            case 'n':
                clear_row(INPUT_ROW);
                clear_row(OUTPUT_ROW);
                curr_step = find_next_step(steps, curr_step);
                clear_buffer(&output_buff);
                write_step(&output_buff, curr_step);
                print_at_row(OUTPUT_ROW, output_buff.data);
                progress_current++;
                break;
        }
        fflush(stdout);
    }
    clear_screen();
    move_to_row(FIRST_ROW);
    free(steps);
    return 0;
}
