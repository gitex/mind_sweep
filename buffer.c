#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include "buffer.h"


Buffer buf_new(size_t size) {
    Buffer buf = {0};

    buf.data = calloc(size, sizeof(char));
    if (!buf.data) {
        buf.size = 0;
        return buf;
    }

    buf.size = size;
    return buf;
}

size_t buf_used(Buffer *buf) {
    if (!buf || !buf->data) return (size_t)0;

    return strlen(buf->data);
}

size_t buf_left(Buffer *buf) {
    if (!buf || !buf->data) return (size_t)0;

    return buf->size - buf_used(buf);
}

// buf_append(buf, "something");
// buf_append(buf, "%s", something);
bool buf_append(Buffer *buf, const char *format, ...) {
    if (!buf || !buf->data || buf->size == 0) return false;

    size_t used = buf_used(buf);
    size_t left = buf_left(buf);

    va_list ap;
    va_start(ap, format);
    int n = vsnprintf(buf->data + used, left, format, ap);
    va_end(ap);

    return (n >= 0) && ((size_t)n < left);
}

void buf_clean(Buffer *buf) {
    if (!buf || !buf->data) return;

    memset(buf->data, 0, buf->size);
}
