#ifndef PICOC_H
#define PICOC_H

/* picoc version number */
#define PICOC_VERSION "v2.1"

/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#ifdef UNIX_HOST
#include <setjmp.h>

/* mark where to end the program for platforms which require this */
extern jmp_buf PicocExitBuf;

/* this has to be a macro, otherwise errors will occur due to the stack being corrupt */
#define PicocPlatformSetExitPoint() setjmp(PicocExitBuf)
#endif

#ifdef SURVEYOR_HOST
/* mark where to end the program for platforms which require this */
extern int PicocExitBuf[];

#define PicocPlatformSetExitPoint() setjmp(PicocExitBuf)
#endif

#ifdef __cplusplus
#define __extern_c extern "C"
#else
#define __extern_c
#endif

/* parse.c */
__extern_c void PicocParse(const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource);
__extern_c void PicocParseInteractive();

/* platform.c */
__extern_c void PicocCallMain(int argc, char **argv);
__extern_c void PicocInitialise(int StackSize);
__extern_c void PicocCleanup();
__extern_c void PicocPlatformScanFile(const char *FileName);

extern int PicocExitValue;

/* include.c */
__extern_c void PicocIncludeAllSystemHeaders();

#endif /* PICOC_H */
