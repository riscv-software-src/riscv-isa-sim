// write a c header and source to read arguments from a file.
// it should return a pointer to a struct containing int argc and char** argv fields.
// chatGPT:

#include "args_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_header.h"

// Helper function to count the number of arguments in a file
static int count_args(const char *str)
{
    int count = 0;
    const char *p = str;

    while (*p)
    {
        while (*p && *p == ' ')
            p++; // Skip spaces
        if (*p)
            count++;
        while (*p && *p != ' ')
            p++; // Skip non-spaces
    }

    return count;
}

// Function to read arguments from a file
argv_argc_t *read_args_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        DEBUG_PRINT_WARN("Failed to open file %s\n", filename);
        return NULL;
    }
    DEBUG_PRINT_WARN("filename = %s, file pointer = %p\n", filename, file);
    printf("reading args from file: %s\n", filename);
    // Read file content
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char *)malloc(length + 1);
    if (!content)
    {
        DEBUG_PRINT_WARN("couldn't allocate space to read file content\n");
        fclose(file);
        return NULL;
    }
    fread(content, 1, length, file);
    // print the content that is read from the file
    printf("\n--running cosim with arguments:--\n %s\n"
    "------------------------------------------\n",
    content);
    fclose(file);
    content[length] = '\0';

    // Count arguments
    int argc = count_args(content);
    DEBUG_PRINT_WARN("argc = %d\n", argc);

    char **argv = (char **)malloc(sizeof(char *) * argc);
    if (!argv)
    {
        DEBUG_PRINT_WARN("couldn't allocate space for argv pointers\n");
        free(content);
        return NULL;
    }

    // Parse arguments
    char *token = strtok(content, " ");
    int i = 0;
    while (token)
    {
        DEBUG_PRINT_WARN("token[%d] = %s\n",i, token);
        argv[i++] = strdup(token);
        token = strtok(NULL, " ");
    }

    // Create and populate argv_argc_t
    argv_argc_t *args = (argv_argc_t *)malloc(sizeof(argv_argc_t));
    if (!args)
    {
        for (int j = 0; j < i; j++)
            free(argv[j]);
        free(argv);
        free(content);
        return NULL;
    }
    args->argc = argc;
    args->argv = argv;

    // Free the original content as strdup made copies
    free(content);

    return args;
}
