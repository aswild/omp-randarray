#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

typedef struct {
    char suf;
    double val;
} suffix_t;

static const suffix_t suffs[] = {
    { 'k', 1000.0 },
    { 'm', 1000000.0 },
    { 'g', 1000000000.0 },
    { 'K', (double)(1L << 10) },
    { 'M', (double)(1L << 20) },
    { 'G', (double)(1L << 30) },
    { '\0', 0.0 },
};

double get_hsize(const char *str)
{
    char *end;
    double val = strtod(str, &end);

    while (*end && isspace(*end))
        end++;

    if (!*end)
        return val;

    for (int i = 0; suffs[i].suf; i++)
    {
        if (*end == suffs[i].suf)
        {
            val *= suffs[i].val;
            return val;
        }
    }

    fprintf(stderr, "Unknown size suffix '%s'\n", end);
    return -1.0;
}
