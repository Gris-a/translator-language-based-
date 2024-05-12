#ifndef SOURCE_H
#define SOURCE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

struct Source
{
    size_t n_lines;
    char **lines;

    char *buffer;
};

Source *ReadSource(const char *file_name);

Source *FreeSource(Source *src);

#endif //SOURCE_H