
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <mutex>
#include <thread>

#include <time.h>
#ifdef WIN32
#include <windows.h>
#include <winsock.h>

#else
#include <sys/time.h>
#include <dirent.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#include "logger.h"
#endif

#define DEFAULT_LOG_FILENAME "tinylog.log"
#define MB (1024 * 1024)
#define MAX_LOG_FILESIZE (50 * MB)
#define MAX_BUF_LEN 4096
#define MAX_LOG_LEN (MAX_BUF_LEN - 1)
#define MAX_LOG_LEVEL 7
#define LOG_STDOUT 0

#ifdef __ANDROID__
#define LOG_STDOUT 1
// #define LOG_RAW_STRING 1
#endif

typedef std::unique_lock<std::mutex> Autolock;
typedef void (*LogCallbackFunc) (const char* tag, int level, const char* msg);

struct Logger
{
    char *filename_;
    char *filepath_;
    FILE *fp_;
    long filesize_;
    char buf[MAX_LOG_LEN];
    bool force_flush_;
    bool stdout_;
    int max_log_level_;
    LogCallbackFunc log_callback_;
};

static Logger *s_logger = NULL;
static std::mutex s_log_mutex;
static std::once_flag init_flag;

#ifdef WIN32
static int gettimeofday(struct timeval *tp, void *tzp) {
    time_t clock;
    struct tm tm;
    SYSTEMTIME wt;
    
    GetLocalTime(&wt);
    tm.tm_year = wt.wYear - 1900;
    tm.tm_mon = wt.wMonth - 1;
    tm.tm_mday = wt.wDay;
    tm.tm_hour = wt.wHour;
    tm.tm_min = wt.wMinute;
    tm.tm_sec = wt.wSecond;
    tm.tm_isdst = -1;

    clock = mktime(&tm);
    tp->tv_sec = (long) clock;
    tp->tv_usec = wt.wMilliseconds * 1000;
    
    return 0;
}

DWORD pthread_self() {
    return ::GetCurrentThreadId();
}
#endif // WIN32

void logger_log(const char* tag, int level, const char* fmt, ...);

static int logger_init_once(const char *path, const char *filename)
{
    Autolock lock(s_log_mutex);
    if (s_logger == NULL)
    {
        s_logger = new Logger;
        s_logger->log_callback_ = NULL;
        if (path) {
            char buf[1024] = { 0 };
            const char* name = NULL;
            if (filename == NULL) {
                name = DEFAULT_LOG_FILENAME;
            }
            else {
                name = filename;
            }
            if (path[strlen(path) - 1] == '/') {
                sprintf(buf, "%s%s", path, name);
            }
            else {
                sprintf(buf, "%s/%s", path, name);
            }

            FILE *fp = fopen(buf, "a");
            if (fp)
            {
                s_logger->filesize_ = ftell(fp);
                s_logger->fp_ = fp;
#ifdef _WIN32
                s_logger->filename_ = _strdup(buf);
                s_logger->filepath_ = _strdup(path);
#else
                s_logger->filename_ = strdup(buf);
                s_logger->filepath_ = strdup(path);
#endif
                s_logger->force_flush_ = true;
                s_logger->stdout_ = false;
            }
            else
            {
#ifdef __ANDROID__
                __android_log_print(ANDROID_LOG_ERROR, "LOGGER", "can't create log file"); 
#else
                fprintf(stderr, "can't create log file");
#endif
                return -1;
            }
        }
        else {
            s_logger->filesize_ = 0;
            s_logger->fp_ = NULL;
            s_logger->filename_ = NULL;
            s_logger->filepath_ = NULL;
            s_logger->stdout_ = true;
        }

        s_logger->max_log_level_ = MAX_LOG_LEVEL;
#if LOG_STDOUT
        s_logger->stdout_ = true;
#endif
    }
    else
    {
        fprintf(stderr, "log file has been inited");
        return -2;
    }

    return 0;
}

int logger_init(const char *path, const char *name)
{
    std::call_once(init_flag, [path, name]() { logger_init_once(path, name); });
    return 0;
}

void logger_destroy()
{
    Autolock lock(s_log_mutex);
    if (s_logger) {
        if (s_logger->fp_) 
            fclose(s_logger->fp_);

        if (s_logger->filename_)
            free(s_logger->filename_);

        if (s_logger->filepath_)
            free(s_logger->filepath_);

        delete s_logger;
        s_logger = NULL;
    }
}

static void logger_rotate_l()
{
    if (!s_logger)
        return;
#ifdef _WIN32

#else
    struct dirent *de = NULL;
    int count = 0;
    DIR *dir = opendir(s_logger->filepath_);

    if (dir)
    {
        do {
            de = readdir(dir);
            if (de) {
                if (strstr(de->d_name, DEFAULT_LOG_FILENAME)) {
                    count++;
                }
            }
        } while (de);
        closedir(dir);
    }

    fclose(s_logger->fp_);

    char buf[1024] = { 0 };
    sprintf(buf, "%s.%d", s_logger->filename_, count);
    std::rename(s_logger->filename_, buf);

    s_logger->filesize_ = 0;
    s_logger->fp_ = fopen(s_logger->filename_, "a");
#endif
}

static const char *log_level_str[] = {"fatal", "error", "warn", "info", "debug", "verbose", ""};
// static const char *log_level_str[] = { "F", "E", "W", "I", "D", "V" };


static inline const char * current_datetime() 
{
    static char timestr[32];
    char tmbuf[32];
    struct tm *nowtm;
    struct timeval tv;
    time_t now_time;

    gettimeofday(&tv, NULL);
    now_time = tv.tv_sec;
    nowtm = localtime(&now_time);
    strftime(tmbuf, sizeof(tmbuf)/sizeof(tmbuf[0]), "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(timestr, sizeof(timestr)/sizeof(timestr[0]), "%s.%03ld", tmbuf, tv.tv_usec/1000);

    return timestr;
}

#ifdef __ANDROID__
static void logger_android_print(const char *tag, int level, const char *msg)
{
    int android_level = ANDROID_LOG_DEBUG;
    switch (level) {
        case 0:
            android_level = ANDROID_LOG_FATAL;
            break;
        case 1:
            android_level = ANDROID_LOG_ERROR;
            break;
        case 2:
            android_level = ANDROID_LOG_WARN;
            break;
        case 3:
            android_level = ANDROID_LOG_INFO;
            break;
        case 4:
            android_level = ANDROID_LOG_DEBUG;
            break;
        case 5:
            android_level = ANDROID_LOG_VERBOSE;
            break;
        default:
            android_level = ANDROID_LOG_DEBUG;
            break;
    }

    __android_log_print(android_level, tag, "%s", msg);   
}
#endif

void logger_write(const char *tag, int level, const char *fmt, va_list args)
{
    if (s_logger == NULL) {
       logger_init(NULL, NULL);
        // return;
    }

    if (s_logger)
    {
        if (s_logger->log_callback_) {
            vsnprintf(s_logger->buf, MAX_LOG_LEN - 1, fmt, args);
            s_logger->log_callback_(tag, level, s_logger->buf);
        }
        else
        {
            if (level < s_logger->max_log_level_) {
                Autolock lock(s_log_mutex);
                int len = 0;

                if (s_logger->filesize_ > MAX_LOG_FILESIZE)
                {
                    logger_rotate_l();
                }
#ifndef LOG_RAW_STRING
                const char* timestr = current_datetime();
#ifdef __APPLE__
                len = snprintf(s_logger->buf, MAX_LOG_LEN, "%s %8s %20s %p\t",
                    timestr, log_level_str[level], tag, pthread_self());
#elif _WIN32
                len = snprintf(s_logger->buf, MAX_LOG_LEN, "%s %8s %20s %lu\t",
                    timestr, log_level_str[level], tag, (unsigned long)pthread_self());
#else
                len = snprintf(s_logger->buf, MAX_LOG_LEN, "%s %8s %20s %lu\t",
                    timestr, log_level_str[level], tag, pthread_self());
#endif
#endif
                vsnprintf(s_logger->buf + len, MAX_LOG_LEN - len, fmt, args);
                if (s_logger->fp_) {
                    s_logger->filesize_ += fprintf(s_logger->fp_, "%s\n", s_logger->buf);

                    if (s_logger->force_flush_)
                    {
                        fflush(s_logger->fp_);
                    }
                }

                if (s_logger->stdout_) {
                    fprintf(stdout, "%s\n", s_logger->buf);
                }
#ifdef __ANDROID__
                logger_android_print(tag, level, s_logger->buf);
#endif
            }
        }
    }
}

void logger_set_callback(void(*callback)(const char*, int, const char*))
{
    if (s_logger) {
        s_logger->log_callback_ = callback;
    }
}

void logger_hexdump(const char *tag, int level, void *data, int size)
{
    int len = 0;
    char line[64] = { 0 };
    int index = 0;
    int offset = 0;

    while (offset < size && len < sizeof(line)) {
        len += sprintf(line, "%08x  ", offset);
        int count = 16;
        while (offset < size && --count >= 0) {
            len += sprintf(line + len, "%02X ", 0xFF & ((uint8_t *)data)[offset]);
            if (count == 8) {
                len += sprintf(line + len, "   ");
            }
            ++offset;
        }

        if (len > 0) {
            //line[len] = 0;
            logger_log(tag, level, "%s", line);
            len = 0;
        }
    }
}

void logger_set_level(int level) {
    if (s_logger) {
        s_logger->max_log_level_ = level;
    }
}

void logger_log(const char *tag, int level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    logger_write(tag, level, fmt, args);
    va_end(args);
}

FILE* dump_start(const char* path)
{
    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        logger_log("LOGGER_DUMP", 1, "failed to create dump file: %s", strerror(errno));
        return NULL;
    }

    return fp;
}

size_t dump_data(FILE* fp, void* data, size_t len)
{
    if (fp)
        return fwrite(data, 1, len, fp);
    return 0;
}

void dump_end(FILE* fp)
{
    if (fp)
        fclose(fp);
}