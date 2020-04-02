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

#include "sfmalloc.h"

#define NUM_THREADS           8
#define MAX_NUM_POINTERS   5000

/**
 * Allocates a bunch of memory and frees
 *
 * Since multiples threads are running at the same time,
 * it's expected to stress the locks and thus, guarantees
 * that safemalloc is behaving as expected.
 */
void* do_work(void *ptr)
{
	((void)ptr);
	char **ptr_vec = sf_malloc(sizeof(char *) * MAX_NUM_POINTERS);

	for (int i = 0; i < MAX_NUM_POINTERS; i++)
	{
		ptr_vec[i] = sf_malloc(256);
		ptr_vec[i][0] = 'A';
	}
	for (int i = 0; i < MAX_NUM_POINTERS; i++)
		sf_free(ptr_vec[i]);

	sf_free(ptr_vec);
	return (NULL);
}

/**
 * Main routine.
 */
int main(int argc, char **argv)
{
	((void)argc);
	pthread_t threads[NUM_THREADS];

	/* Initialize safemalloc. */
	sf_init(VERBOSE_MODE_2, ON_ERROR_ABORT);

	/* Spawn threads. */
	for (int i = 0; i < NUM_THREADS; i++)
	{
		if (pthread_create(&threads[i], NULL, do_work, NULL))
		{
			fprintf(stderr, "%s: failed while creating thread #%d\n",
				argv[0], i);
			exit(EXIT_FAILURE);
		}
	}

	/* Wait. */
	for (int i = 0; i < NUM_THREADS; i++)
	{
		if (pthread_join(threads[i], NULL))
		{
			fprintf(stderr, "%s: failed while joining thread #%d\n",
				argv[0], i);
			exit(EXIT_FAILURE);
		}
	}

	return (0);
}
