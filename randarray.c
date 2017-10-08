#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include "timer.h"

#ifdef WITH_OPENMP
#include <omp.h>
#else
// stubs for compiling without omp
#define omp_get_thread_num() 0
#define omp_set_num_threads(...)
#endif

#ifdef WITH_MD5
#include <openssl/md5.h>
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

extern double get_hsize(const char *str);

static void usage(void)
{
    static const char usage_text[] =
        "Usage: randarray OPTIONS\n"
        "\n"
        "Arguments:\n"
        "  -n SIZE      The size of the array in bytes. Should be a multiple of 4\n"
        "               (suffixes k,m,g,K,M,G for SI and Binary units can be used)\n"
        "  -s SEED      The initial seed for repeatable runs\n"
        "  -T THREADS   number of threads\n"
        "  -q           Run quietly and print just the number of seconds to stdout\n"
        "  -m           Print the md5 of the generated data\n"
        ;
    puts(usage_text);
}

int main(int argc, char *argv[])
{
    int nthreads = 1;
    size_t arr_size = 1024;
    bool quiet = false;
    unsigned int start_seed = time(NULL);
#ifdef WITH_MD5
    bool use_md5 = false;
#endif

    unsigned int *seeds;
    int32_t *arr;
    atimer_t timer;

    // parse arguments
    int opt;
    while ((opt = getopt(argc, argv, "hmn:qs:T:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                usage();
                exit(0);
                break;

            case 'm':
#ifdef WITH_MD5
                use_md5 = true;
#else
                fprintf(stderr, "no MD5 (openssl) support compiled in\n");
#endif
                break;

            case 'n':
                {
                    double size_bytes = get_hsize(optarg);
                    if (size_bytes < 0)
                        exit(1);
                    arr_size = lround(size_bytes / (double)(sizeof(*arr)));
                }
                break;

            case 'q':
                quiet = true;
                break;

            case 's':
                start_seed = strtoul(optarg, NULL, 0);
                break;

            case 'T':
#ifdef WITH_OPENMP
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
    srandom(start_seed);
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

#ifdef WITH_MD5
    if (use_md5)
    {
        unsigned char md[16];
        if (!quiet)
            printf("calculating MD5...\n");

        timer_init(&timer);
        timer_start(&timer);
        MD5((const unsigned char *)arr, arr_size * sizeof(*arr), md);
        timer_stop(&timer);

        for (int i = 0; i < 16; i++)
            printf("%02x", (unsigned int)md[i]);
        printf("\n");

        if (!quiet)
        {
            printf("md5 calculation took ");
            timer_print(&timer, stdout);
            printf("\n");
        }
    }
#endif

    free(arr);
    return 0;
}
