#include "interpreter.h"

/* deallocate any storage */
void PlatformCleanup()
{
}

/* get a line of interactive input */
char *PlatformGetLine(char *Buf, int MaxLen)
{
    // XXX - unimplemented so far
    return NULL;
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
    // XXX - unimplemented so far
    return 0;
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    // XXX - unimplemented so far
}

/* read a file into memory */
char *PlatformReadFile(const char *FileName)
{
    // XXX - unimplemented so far
    return NULL;    
}

/* read and scan a file for definitions */
void PlatformScanFile(const char *FileName)
{
    char *SourceStr = PlatformReadFile(FileName);
    Parse(FileName, SourceStr, strlen(SourceStr), TRUE);
    //free(SourceStr);
}

/* mark where to end the program for platforms which require this */
jmp_buf ExitBuf;

/* exit the program */
void PlatformExit()
{
    longjmp(ExitBuf, 1);
}

