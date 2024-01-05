// write a c header and source to read arguments from a file. 
// it should return a pointer to a struct containing int argc and char** argv fields.
// chatGPT:

#ifndef ARGS_READER_H
#define ARGS_READER_H

// Structure to hold argc and argv
typedef struct {
    int argc;
    char **argv;
} argv_argc_t;

// Function prototype to read arguments from a file
argv_argc_t *read_args_from_file(const char *filename);

#endif // ARGS_READER_H
