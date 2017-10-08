#include <stdio.h>
#include <math.h>
#include "timer.h"

void timer_print(atimer_t *t, FILE *fp)
{
    double elapsed = ((double)t->end.tv_sec + (double)t->end.tv_nsec / 1000000000.0) -
                     ((double)t->start.tv_sec + (double)t->start.tv_nsec / 1000000000.0);
    double minutes = floor(elapsed / 60.0);
    double seconds = fmod(elapsed, 60.0);
    fprintf(fp, "%02.0f:%08.5f", minutes, seconds);
}
