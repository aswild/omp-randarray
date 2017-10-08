#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "timer.h"

#define RD_STATE_LEN 128
#define CHECKPOINT fprintf(stderr, "%s:%d CHECKPOINT\n", __func__, __LINE__);

void usage(void)
{
    static const char usage_text[] =
        "Usage: randarray [-n SIZE | -N LOGSIZE]\n"
        "\n"
        "Arguments:\n"
        "  -n SIZE      The size of the square array\n"
        "  -N LOGSIZE   The log2 of the size of the square array\n"
        "               (LOGSIZE=10 means 1024x1024)\n"
        ;
    puts(usage_text);
}

int main(int argc, char *argv[])
{
    atimer_t timer;
    unsigned int seed;
    struct random_data *rd;
    char *rd_state;
    int opt;
    size_t arr_size = 1024;
    int32_t *arr;

    while ((opt = getopt(argc, argv, "hn:N:")) != -1)
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
                    arr_size = 1 << pow;
                }
                break;

            default:
                usage();
                exit(1);
                break;
        }
    }

    printf("Using array size %zu (%zu bytes)\n", arr_size, arr_size * sizeof(*arr));
    arr = malloc(arr_size * sizeof(*arr));
    if (!arr)
    {
        printf("malloc failed!\n");
        exit(1);
    }

    timer_init(&timer);
    srandom(time(NULL));
    seed = random();
    rd = calloc(1, sizeof(*rd));
    rd_state = calloc(1, RD_STATE_LEN);
    initstate_r(seed, rd_state, RD_STATE_LEN, rd);
    srandom_r(seed, rd);

    timer_start(&timer);
    for (size_t i = 0; i < arr_size; i++)
    {
        random_r(rd, &arr[i]);
    }
    timer_stop(&timer);

    printf("finished in ");
    timer_print(&timer, stdout);
    printf("\n");

    free(rd);
    free(rd_state);
    free(arr);
    return 0;
}
