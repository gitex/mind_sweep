#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <stdbool.h>
#include "console.h"
#include "file.h"

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
#define LEVEL_MAX                 20
#define LEVEL_SIGN                '#'


typedef enum {
    MS_OK = 0,
    MS_ERR_INPUT = -1,
    MS_ERR_FILE_NOT_FOUND = -2,
    MS_ERR_FILE_EMPTY = -3,
} output_t;


typedef struct Step {
    char name[64];
    int level;
    bool is_leaf;
    struct Step *parent;
    struct Step *next;
} Step;


typedef struct Buffer {
    char *data;
    size_t size;
} Buffer;


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

    // last
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
    strcat(buf->data, suffix);

    strcat(buf->data, ANSI_COLOR_GREEN);
    strcat(buf->data, step->name);
    strcat(buf->data, ANSI_COLOR_RESET);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./main <md_file>\n");
        return MS_ERR_INPUT;
    }
    char *filepath = argv[1];

    FILE *file = file_open(filepath, "r");
    if (!file) {
        perror("Cannot open file\n");
        return MS_ERR_FILE_NOT_FOUND;
    }
    char *file_content = file_read_into_memory(file);
    if (!file_content) {
        perror("Cannot load file into memory\n");
        return MS_ERR_FILE_NOT_FOUND;
    }
    fclose(file);

    // read file content
    Step *steps = (Step*)malloc(STEPS_MAX_SIZE * sizeof(Step));
    if (!steps) {
        printf("Cannot allocate the memory");
        free(file_content);
        return -1;
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
        Step *curr = &steps[idx];

        if (*start != LEVEL_SIGN) {
            start = end + 1;
            continue;
        }

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

        curr->parent = parents_cache[curr->level - 1];

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
    prev->is_leaf = true;
    leafs_count++;

    free(file_content);

    // --- TUI ---
    clear_screen();
    int quit = 0;

    char *commands[2][2] = {
        {"n", "next"},
        {"q", "exit"},
    };
    size_t commands_size = sizeof(commands) / sizeof(*commands);
    Step *current = NULL;
    bool is_completed = false;
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
            if ((size_t)i != commands_size - 1) { printf(", "); }
        }

        clear_buffer(&progress_buff);
        write_progress(&progress_buff, progress_current, leafs_count);
        print_at_row(PROGRESS_ROW, progress_buff.data);
        move_to_row(INPUT_ROW);

        char c = getchar();

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
                clear_buffer(&output_buff);
                write_step(&output_buff, current);
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
