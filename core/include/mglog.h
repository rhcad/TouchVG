// mglog.h

#ifndef TOUCHVG_LOG_H
#define TOUCHVG_LOG_H

#ifndef LOGD

#ifdef __ANDROID__

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"vgjni",__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"vgjni",__VA_ARGS__)

#elif defined(__APPLE__)

#include <asl.h>

#define __CLOG_VARGS_LOG(LEVEL) \
    va_list ap; \
    va_start(ap, fmt); \
    __clog_log(LEVEL, fmt, ap); \
    va_end(ap);

static inline void __clog_log(int level, const char * fmt, va_list ap) {
    aslmsg msg;
    static aslclient __clog_client = (aslclient)0;
    
    if (!__clog_client) {
        __clog_client = asl_open("touchvg", "com.apple.console", ASL_OPT_STDERR | ASL_OPT_NO_DELAY);
    }
    if (__clog_client) {
        msg = asl_new(ASL_TYPE_MSG);
        asl_set(msg, ASL_KEY_FACILITY, "com.apple.console");
        asl_vlog(__clog_client, msg, level, fmt, ap);
        asl_free(msg);
    }
}

inline void LOGD(const char * fmt, ...) {
    __CLOG_VARGS_LOG(ASL_LEVEL_DEBUG);
}
inline void LOGE(const char * fmt, ...) {
    __CLOG_VARGS_LOG(ASL_LEVEL_ERR);
}

#else
#define NO_LOGD
#define LOGD(...)
#define LOGE(...)
#endif

#endif // LOGD

#endif // TOUCHVG_LOG_H
