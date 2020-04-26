#ifndef LOGGER_H

#include <stdarg.h>
#include <stdio.h>

enum {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
};

int  logger_init(const char *path, const char *name);
void logger_set_level(int level);
void logger_destroy();
void logger_log(const char *tag, int level, const char *fmt, ...);
void logger_write(const char *tag, int level, const char *fmt, va_list args);
void logger_set_callback(void (*callback)(const char*, int , const char*));

void logger_hexdump(const char *tag, int level, void *data, int size);

FILE*  dump_start(const char* path);
size_t dump_data(FILE* fp, void* data, size_t len);
void   dump_end(FILE* fp);

#endif /* LOGGER_H */