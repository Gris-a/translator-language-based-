#include "source.h"


static size_t FileSize(const char *file_name);
static char *BuffSource(const char *file_name);

static size_t LinesCnt(char *buffer);
static void SourceLinearize(Source *src);


static size_t FileSize(const char *file_name)
{
    struct stat file_info = {};
    stat(file_name, &file_info);

    return (size_t)file_info.st_size;
}

static char *BuffSource(const char *file_name)
{
    FILE *file = fopen(file_name, "rb");
    assert(file);

    size_t buffer_sz = FileSize(file_name);
    char *buffer = (char *)calloc(buffer_sz + 1, sizeof(char));
    assert(buffer);

    fread(buffer, sizeof(char), buffer_sz, file);
    fclose(file);

    return buffer;
}


static size_t LinesCnt(char *buffer)
{
    size_t n_lines = 0;

    if(*buffer == '\0') return n_lines;
    while(*buffer != '\0')
    {
        if(*(buffer++) == '\n') n_lines++;
    }
    if(*(buffer - 1) != '\n') n_lines++;

    return n_lines;
}


static void SourceLinearize(Source *src)
{
    char *buf_p = src->buffer;

    src->lines[0] = buf_p;
    size_t line_count = 1;

    while(*buf_p != '\0')
    {
        if(*buf_p == '\n' && *(buf_p + 1) != '\0')
        {
            *buf_p = '\0';
            src->lines[line_count++] = buf_p + 1;
        }
        buf_p++;
    }
}


Source *ReadSource(const char *file_name)
{
    Source *src = (Source *)malloc(sizeof(Source));
    assert(src);

    src->buffer = BuffSource(file_name);
    assert(src->buffer);

    src->n_lines = LinesCnt(src->buffer);
    assert(src->n_lines);

    src->lines = (char **)calloc(src->n_lines, sizeof(char *));
    assert(src->lines);

    SourceLinearize(src);

    return src;
}


Source *FreeSource(Source *src)
{
    free(src->lines);
    free(src->buffer);
    free(src);

    return NULL;
}