AM_CFLAGS = -Wall -Wextra -Werror
if WITH_OPENMP
AM_CFLAGS += -fopenmp
else
AM_CFLAGS += -Wno-unknown-pragmas
endif

bin_PROGRAMS = randarray
randarray_SOURCES = randarray.c \
					timer.c

randarray_LDADD = -lm
