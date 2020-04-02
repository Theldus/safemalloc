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

/*
 * Safe Malloc Memory Estimator.
 */
#ifdef SFMALLOC_SAVE_SPACE
int save_space_enabled = 1;
#else
int save_space_enabled = 0;
#endif

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "sfmalloc.h"

/**
 * Rounds up to the next power of two.
 * @param target Target number to be rounded.
 * @returns The next power of two.
 */
static size_t roundPower(size_t target)
{
	target--;
	target |= target >> 1;
	target |= target >> 2;
	target |= target >> 4;
	target |= target >> 8;
	target |= target >> 16;
	target++;
	return (target);
}

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
 * Usage.
 */
static void usage(const char *p)
{
	fprintf(stderr, "%s [number-of-elements] [size-per-element]\n", p);
	exit(EXIT_FAILURE);
}

/**
 * Main routine.
 */
int main (int argc, char **argv)
{
	int num_elements;
	int size_per_element;
	size_t buckets;

	size_t ideal_size;
	size_t total_size;
	double factor;

	if (argc < 3)
		usage(argv[0]);

	if (str2int(&num_elements, argv[1]) < 0)
		usage(argv[0]);
	if (str2int(&size_per_element, argv[2]) < 0)
		usage(argv[0]);

	ideal_size = (num_elements * size_per_element);

	buckets = roundPower(num_elements);
	if (buckets > buckets*HASHTABLE_LOAD_FACTOR)
		buckets <<= 1;

	total_size = (
			/* Hash table structure, allocated once. */
			 sizeof(struct hashtable) +

			/* Each address list. */
			(sizeof(struct address) * num_elements) +

			/* Buckets list. */
			(sizeof(void *) * buckets) +

			/* Size desired. */
			(ideal_size)
		);

	factor = (double)total_size/ideal_size;

	printf("Save space is enabled?: %d\n", save_space_enabled);
	printf("Total size: %zu bytes\n"
		"Ideal size: %zu bytes\n"
		"Factor: %f\n",
		total_size, ideal_size, factor);
}
