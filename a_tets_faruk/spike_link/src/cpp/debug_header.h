#ifndef DEBUG_HEADER_H
#define DEBUG_HEADER_H

#define NO_DEBUG 0
#define DEBUG 1
#define DEBUG_LEVEL NO_DEBUG

// instead of doing a runtime if, we do a compile time if
// using preprocessor macros.

#if DEBUG_LEVEL > 0
#define DEBUG_PRINT(fmt, ...) \
  do \
  { \
    fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
            __LINE__, __func__, ##__VA_ARGS__); \
    fflush(stderr); \
  } while (0)

#else
#define DEBUG_PRINT(fmt, ...) \
  do { \
  } while (0)

#endif


#endif // DEBUG_HEADER_H
