#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#ifdef SHELL
#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#include <android/log.h>
#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, "exploit", fmt, ##__VA_ARGS__)
#endif

#endif
