AM_CFLAGS = -Wall -Wextra -Werror
if WITH_OPENMP
AM_CFLAGS += -fopenmp
else
AM_CFLAGS += -Wno-unknown-pragmas
endif

AM_LDFLAGS = -Wl,--as-needed -Wl,-O2

noinst_PROGRAMS = randarray
randarray_SOURCES = randarray.c \
                    timer.c \
                    hsize.c

randarray_LDADD = -lm
if WITH_OPENSSL
randarray_LDADD += -lcrypto
endif
