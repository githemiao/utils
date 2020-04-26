#ifndef LOG_H
#define LOG_H

#define LOG_COMPLEX 1
#define LOG_TRACE 1

#ifndef TAG
#define TAG ""
#endif


#include "logger.h"

#define LOG_LEVEL_VERBOSE 5
#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_FATAL   0

#ifdef LOGF
#undef LOGF
#endif

#ifdef LOGE
#undef LOGE
#endif

#ifdef LOGW
#undef LOGW
#endif

#ifdef LOGD
#undef LOGD
#endif

#ifdef LOGI
#undef LOGI
#endif

#ifdef LOGV
#undef LOGV
#endif

#if LOG_COMPLEX
#define LOG(tag, level, fmt, ...) logger_log(tag, level, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGF(tag, fmt, ...) logger_log(tag, 0, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGE(tag, fmt, ...) logger_log(tag, 1, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGW(tag, fmt, ...) logger_log(tag, 2, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGI(tag, fmt, ...) logger_log(tag, 3, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGD(tag, fmt, ...) logger_log(tag, 4, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGV(tag, fmt, ...) logger_log(tag, 5, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(tag, level, fmt, ...) logger_log(tag, level, fmt, ##__VA_ARGS__)

#define LOGF(tag, fmt, ...) logger_log(tag, 0, fmt, ##__VA_ARGS__)
#define LOGE(tag, fmt, ...) logger_log(tag, 1, fmt, ##__VA_ARGS__)
#define LOGW(tag, fmt, ...) logger_log(tag, 2, fmt, ##__VA_ARGS__)
#define LOGI(tag, fmt, ...) logger_log(tag, 3, fmt, ##__VA_ARGS__)
#define LOGD(tag, fmt, ...) logger_log(tag, 4, fmt, ##__VA_ARGS__)
#define LOGV(tag, fmt, ...) logger_log(tag, 5, fmt, ##__VA_ARGS__)
#endif

#if LOG_TRACE
#define TRACE() LOGV("TRACE", "%s:%d:%s", __FILE__, __LINE__, __FUNCTION__)
#else
#define TRACE()
#endif


#endif // LOG_H
