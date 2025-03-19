#ifdef LOGGING_ENABLED
#define LOGGING_ENABLED 1
#else
#define LOGGING_ENABLED 0
#endif

#define debug_log(fmt, ...) \
        do { if (LOGGING_ENABLED) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__ ); } while (0)
