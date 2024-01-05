#ifndef DEBUG_HEADER_H
#define DEBUG_HEADER_H

#define DEBUG_NONE 0
#define DEBUG_INFO 1
#define DEBUG_WARN 2
#define DEBUG_ERROR 3
#define DEBUG_FATAL 4
#define DEBUG_LEVEL DEBUG_NONE

// instead of doing a runtime if, we do a compile time if
// using preprocessor macros.

#if DEBUG_LEVEL >= DEBUG_WARN
#define DEBUG_PRINT_WARN(fmt, ...)                \
  do                                              \
  {                                               \
    fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
            __LINE__, __func__, ##__VA_ARGS__);   \
    fflush(stderr);                               \
  } while (0)

#else
#define DEBUG_PRINT_WARN(fmt, ...) \
  do                               \
  {                                \
  } while (0)

#endif

#if DEBUG_LEVEL >= DEBUG_INFO
#define DEBUG_PRINT_INFO(fmt, ...)                \
  do                                              \
  {                                               \
    fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
            __LINE__, __func__, ##__VA_ARGS__);   \
    fflush(stderr);                               \
  } while (0)
#else
#define DEBUG_PRINT_INFO(fmt, ...) \
  do                               \
  {                                \
  } while (0)
#endif

#endif // DEBUG_HEADER_H
