/*
 * MIT License
 *
 * Copyright (c) 2020 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include "sfmalloc.h"

#define ITERATIONS           10
#define MAX_NUM_POINTERS 100000

static int NUM_POINTERS = 0;
char *ptr_vec[MAX_NUM_POINTERS];

/**
 * Safe string-to-int routine that takes into account:
 * - Overflow and Underflow
 * - No undefined behaviour
 *
 * Taken from https://stackoverflow.com/a/12923949/3594716
 * and slightly adapted: no error classification, because
 * I dont need to know, error is error.
 *
 * @param out Pointer to integer.
 * @param s String to be converted.
 *
 * @return Returns 0 if success and a negative number otherwise.
 */
static int str2int(int *out, char *s)
{
	char *end;
	if (s[0] == '\0' || isspace(s[0]))
		return (-1);
	errno = 0;

	long l = strtol(s, &end, 10);

	/* Both checks are needed because INT_MAX == LONG_MAX is possible. */
	if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
		return (-1);
	if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
		return (-1);
	if (*end != '\0')
		return (-1);

	*out = l;
	return (0);
}

/**
 * Get current time in microseconds
 *
 * @return Returns current time in microseconds.
 */
long getTimeMicro(void)
{
	long timestamp = 0;
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC_RAW, &time);
	timestamp = (time.tv_sec * 1000000) + (time.tv_nsec / 1000);
	return (timestamp);
}

/**
 * Benchmark safemalloc.
 */
void bench_safe_malloc(void)
{
	for (int i = 0; i < NUM_POINTERS; i++)
	{
		ptr_vec[i] = sf_malloc(256);
		ptr_vec[i][0] = 'A';
	}
	sf_free_all();
}

/**
 * Benchmark malloc.
 */
void bench_malloc(void)
{
	for (int i = 0; i < NUM_POINTERS; i++)
	{
		ptr_vec[i] = malloc(256);
		ptr_vec[i][0] = 'A';
	}
	for (int i = 0; i < NUM_POINTERS; i++)
		free(ptr_vec[i]);
}

/**
 * Usage.
 */
static void usage(const char *p)
{
	fprintf(stderr, "Usage: %s [number-of-allocations] (below or eq 100000)"
		" [test-number]\n", p);
	fprintf(stderr, "where [test-number]: 1 - sfmalloc / 2 - malloc\n");
	exit(EXIT_FAILURE);
}

/**
 * Main routine.
 */
int main(int argc, char **argv)
{
	long start;
	long end;
	long malloc_time = 0;
	int test;

	if (argc < 3)
		usage(argv[0]);

	if (str2int(&NUM_POINTERS, argv[1]) < 0 || NUM_POINTERS > 100000)
		usage(argv[0]);

	if (str2int(&test, argv[2]) < 0 || test < 1 || test > 2)
		usage(argv[0]);

	/* Initialize safemalloc. */
	sf_init(VERBOSE_MODE_2, ON_ERROR_ABORT);

	/*
	 * Note:
	 * Its important to keep separated these two tests
	 * because since both use malloc (sfmalloc is just
	 * a wrapper...) the first being executed could
	 * 'help' the second...
	 */

	/* Benchmark xx_malloc. */
	if (test == 1)
	{
		for (int i = 0; i < ITERATIONS; i++)
		{
			start = getTimeMicro();
				bench_safe_malloc();
			end = getTimeMicro();
			malloc_time += (end - start);
		}
		malloc_time /= ITERATIONS;
	}
	else
	{
		for (int i = 0; i < ITERATIONS; i++)
		{
			start = getTimeMicro();
				bench_malloc();
			end = getTimeMicro();
			malloc_time += (end - start);
		}
		malloc_time /= ITERATIONS;
	}

	/* Results. */
	if (test == 1)
		printf("sf_malloc: %ld\n", malloc_time);
	else
		printf("malloc: %ld\n", malloc_time);

	return (0);
}
