#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "timer.h"

#if WITH_OPENMP
#include <omp.h>
#else
// stubs for compiling without omp
#define omp_get_thread_num() 0
#define omp_set_num_threads(...)
#endif

#define RD_STATE_LEN 128
#define CHECKPOINT fprintf(stderr, "%s:%d CHECKPOINT\n", __func__, __LINE__);

#define MALLOC(var, size) do { \
    var = malloc(size); \
    if (var == NULL) { \
        fprintf(stderr, "%s: malloc of %zu bytes failed for " #var, __func__, size); \
        exit(1); \
    }} while(0)

#define CALLOC(var, size1, size2) do { \
    var = calloc(size1, size2); \
    if (var == NULL) { \
        fprintf(stderr, "%s: calloc of %lu bytes failed for " #var, __func__, (long unsigned int)(size1 * size2)); \
        exit(1); \
    }} while(0)

void usage(void)
{
    static const char usage_text[] =
        "Usage: randarray OPTIONS\n"
        "\n"
        "Arguments:\n"
        "  -n SIZE      The size of the square array\n"
        "  -N LOGSIZE   The log2 of the size of the square array\n"
        "               (LOGSIZE=10 means 1024x1024)\n"
        "  -T THREADS   number of threads\n"
        "  -q           Run quietly and print just the number of seconds to stdout\n"
        ;
    puts(usage_text);
}

int main(int argc, char *argv[])
{
    int nthreads = 1;
    size_t arr_size = 1024;
    bool quiet = false;

    unsigned int *seeds;
    int32_t *arr;
    atimer_t timer;

    int opt;
    while ((opt = getopt(argc, argv, "hn:N:qT:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                usage();
                exit(0);
                break;

            case 'n':
                arr_size = strtol(optarg, NULL, 0);
                break;

            case 'N':
                {
                    long pow = strtol(optarg, NULL, 0);
                    arr_size = 1L << pow;
                }
                break;

            case 'q':
                quiet = true;
                break;

            case 'T':
#if WITH_OPENMP
                nthreads = strtol(optarg, NULL, 0);
#else
                fprintf(stderr, "compiled without OMP, single-thread only\n");
#endif
                break;

            default:
                usage();
                exit(1);
                break;
        }
    }

    if (!quiet)
    {
        printf("Using %d threads\n", nthreads);
        printf("Using array size %zu (%zu bytes)\n", arr_size, arr_size * sizeof(*arr));
    }
    MALLOC(arr, arr_size * sizeof(*arr));

    // init seeds
    MALLOC(seeds, nthreads * sizeof(*seeds));
    srandom(time(NULL));
    for (int i = 0; i < nthreads; i++)
        seeds[i] = random();

    omp_set_num_threads(nthreads);

    // init and start timer, immediately before parallel region
    timer_init(&timer);
    timer_start(&timer);

/* start threads */
#pragma omp parallel
    {
        // everything defined in this block is thread-private
        // everything defined outside of the parallel region is shared
        struct random_data *rd;
        char *rd_state;
        int tnum = omp_get_thread_num();

        // init prng
        CALLOC(rd, 1, sizeof(*rd));
        CALLOC(rd_state, 1, RD_STATE_LEN);
        initstate_r(seeds[tnum], rd_state, RD_STATE_LEN, rd);
        srandom_r(seeds[tnum], rd);

/* magic happens here */
#pragma omp for
        for (size_t i = 0; i < arr_size; i++)
            random_r(rd, &arr[i]);

        free(rd);
        free(rd_state);
    }
/* end parallel region */

    timer_stop(&timer);
    free(seeds);

    if (quiet)
    {
        timer_print_sec(&timer, stdout);
    }
    else
    {
        printf("finished in ");
        timer_print(&timer, stdout);
    }
    printf("\n");

    free(arr);
    return 0;
}
