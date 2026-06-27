#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "console.h"
#include "file.h"
#include "buffer.h"


#define H_DELIMITER_SIGN          '-'
#define H_DELIMITER_WIDTH         64
#define FIRST_ROW                 1
#define CONSOLE_HELP_LINE         1
#define PROGRESS_ROW              3
#define OUTPUT_ROW                7
#define INPUT_ROW                 9

#define OUTPUT_MAX_SIZE           256
#define STEPS_MAX_SIZE            1024
#define LEVEL_MAX                 20
#define LEVEL_SIGN                '#'


typedef enum {
    MS_OK = 0,
    MS_ERR_INPUT = -1,
    MS_ERR_FILE_NOT_FOUND = -2,
    MS_ERR_FILE_EMPTY = -3,
    MS_ERR_NO_STEPS = -4,
    MS_ERR_ALLOCATION = -5,
    MS_ERR_STEPS_LIMIT = -6,
} output_t;


typedef struct Step {
    char name[64];
    int level;
    bool is_leaf;
    struct Step *parent;
    struct Step *next;
} Step;


Step *find_next_step(Step *steps, Step *current) {
    if (!steps) return NULL;

    Step *curr = current;

    if (curr == NULL) {
        curr = &steps[0];

        if (curr->is_leaf)
            return curr;
    }

    if (!curr->next) {
        return NULL;
    }

    while(curr->next) {
        if (curr->next->is_leaf) {
            return curr->next;
        }
        curr = curr->next;
    }

    return NULL;
}

void write_progress(Buffer *buf, int current, int total) {
    int percent = current * 100 / total;

    buf_append(buf, "progress: [");

    for (int i = 0; i < percent / 2; i++) {
        buf_append(buf, "#");
    }

    for (int i = 0; i < 50 - percent / 2; i++) {
        buf_append(buf, ".");
    }

    buf_append(buf, "] %d/%d", current, total);
}

void write_step(Buffer *buf, Step *step) {
    if (step == NULL) {
        return;
    }

    Step *curr = step;

    char tmp[128] = {0};
    char suffix[128] = {0};
    while (curr->parent) {
        snprintf(tmp, sizeof(tmp), "%s > ", curr->parent->name);
        strcat(tmp, suffix);
        strcpy(suffix, tmp);
        curr = curr->parent;
    }
    buf_append(buf, suffix);
    buf_append(buf, "%s%s%s", ANSI_COLOR_GREEN, step->name, ANSI_COLOR_RESET);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./main <md_file>\n");
        return MS_ERR_INPUT;
    }
    char *filepath = argv[1];

    FILE *file = file_open(filepath, "r");
    if (!file) {
        fprintf(stderr, "Cannot open file\n");
        return MS_ERR_FILE_NOT_FOUND;
    }
    char *file_content = file_read_into_memory(file);
    if (!file_content) {
        fprintf(stderr, "Cannot load file into memory\n");
        fclose(file);
        free(file_content);
        return MS_ERR_FILE_NOT_FOUND;
    }
    fclose(file);

    // read file content
    Step *steps = (Step*)calloc(STEPS_MAX_SIZE, sizeof(Step));
    if (!steps) {
        fprintf(stderr, "Cannot allocate the memory\n");
        free(file_content);
        return MS_ERR_ALLOCATION;
    }

    int idx = 0;
    Step *prev = NULL;
    int leafs_count = 0;
    Step *parents_cache[LEVEL_MAX + 1] = {NULL};

    char *start = file_content;
    char *end;
    while(*start) {
        end = strchr(start, '\n');
        if (end == NULL) {
            break;
        }

        if (*start != LEVEL_SIGN) {
            start = end + 1;
            continue;
        }

        if (idx >= STEPS_MAX_SIZE) break;
        Step *curr = &steps[idx];

        // left trim
        while(*start) {
            if (*start == LEVEL_SIGN) {
                curr -> level++;
                start++;
            } else if (isspace(*start)) {
                start++;
            } else { break; }
        }

        // name
        int len = end - start;
        snprintf(curr->name, sizeof(curr->name), "%.*s", len, start);

        curr->parent = (curr->level > 0) ? parents_cache[curr->level - 1] : NULL;

        if (prev) {
            // next and previous
            prev->next = curr;

            // is_leaf
            if (prev->level >= curr->level) {
                prev->is_leaf = true;
                leafs_count++;
            }
        }

        start = end + 1;
        idx++;
        prev = curr;
        parents_cache[curr->level] = curr;
    }

    // last step is always leaf
    if (prev) {
        prev->is_leaf = true;
        leafs_count++;
    }

    free(file_content);

    if (*steps->name == '\0') {
        fprintf(stderr, "File does not contain any steps.\n");
        free(steps);
        return MS_ERR_NO_STEPS;
    }

    // --- TUI ---
    clear_screen();
    int quit = 0;

    char *commands[3][2] = {
        {"n", "next"},
        {"r", "restart"},
        {"q", "exit"},
    };
    size_t commands_size = sizeof(commands) / sizeof(*commands);
    Step *current = NULL;
    bool is_completed = false;
    int progress_current = 0;
    Buffer progress_buf = buf_new(128);
    Buffer output_buf = buf_new(OUTPUT_MAX_SIZE);

    while(!quit) {
        print_at_row(CONSOLE_HELP_LINE, "help: ");
        for (int i = 0; i < commands_size; i++) {
            print_colored(commands[i][0], ANSI_COLOR_RED);
            printf(" - %s", commands[i][1]);
            if ((size_t)i != commands_size - 1) { printf(", "); }
        }

        buf_clean(&progress_buf);
        write_progress(&progress_buf, progress_current, leafs_count);
        print_at_row(PROGRESS_ROW, progress_buf.data);
        move_to_row(INPUT_ROW);

        char c = getchar();
        if (c == EOF) { quit = 1; break; }

        switch (c) {
            case 'q': quit = 1; break;
            case 'n':
                if (is_completed) break;
                clear_row(INPUT_ROW);
                clear_row(OUTPUT_ROW);
                current = find_next_step(steps, current);
                if (!current) {
                    is_completed = true;
                    break;
                }
                buf_clean(&output_buf);
                write_step(&output_buf, current);
                print_at_row(OUTPUT_ROW, output_buf.data);
                progress_current++;
                break;
            case 'r':
                is_completed = false;
                current = NULL;
                progress_current = 0;
                clear_row(INPUT_ROW);
                clear_row(OUTPUT_ROW);
                clear_row(PROGRESS_ROW);
        }
        fflush(stdout);
    }
    clear_screen();
    move_to_row(FIRST_ROW);
    free(steps);
    return 0;
}
