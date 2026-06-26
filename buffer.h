#ifndef BUFFER
#define BUFFER

#include <stddef.h>

typedef struct Buffer {
    char *data;
    size_t size;
} Buffer;


Buffer buf_new(size_t size);
size_t buf_used(Buffer *buf);
size_t buf_left(Buffer *buf);
bool buf_append(Buffer *buf, const char *format, ...);
void buf_clean(Buffer *buf);


#endif // !__BUFFER__
