#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdio.h>
#include <string.h>
#include <time.h>

#define CLOCK_TO_USE CLOCK_MONOTONIC

typedef struct timer {
    struct timespec start;
    struct timespec end;
} atimer_t;

static inline void timer_init(atimer_t *t)
{
    memset(t, 0, sizeof(*t));
}

static inline int timer_start(atimer_t *t)
{
    return clock_gettime(CLOCK_TO_USE, &t->start);
}

static inline int timer_stop(atimer_t *t)
{
    return clock_gettime(CLOCK_TO_USE, &t->end);
}

void timer_print(atimer_t *t, FILE *fp);

#endif
