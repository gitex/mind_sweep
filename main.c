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

typedef struct Step {
    char name[64];
    int level;
    int order;
    struct Step *parent;
    struct Step *prev;
    struct Step *next;
} Step;


int main(void) {
    // setlocale(LC_ALL, "ru_RU.UTF-8");

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

        // if (idx > 0 && steps[idx - 1].level < curr->level) {
        //     printf("%s -> %s\n", steps[idx - 1].name, curr->name);
        //     // curr->parent = steps[idx - 1];
        // }

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

    for (size_t i = 0; i < steps_size; i++) {
        Step step = steps[i];
        // printf("%s -> %s\n", step.name, step.next->name);

        if (step.next && step.next->parent == &step) {
            if (strlen(parents) > 0) {
                strcat(parents, " -> ");
            }
            strcat(parents, step.name);
        }

        printf("%s", parents);
    }


    free(steps);
    return 0;
}
