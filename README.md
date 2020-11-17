# safemalloc
A two-files library wrapper for [m,c,re]alloc/free functions that intends to
a be safer replacement.

## Motivation
One of the things that 'annoys' in C is the excess of return values that a
programmer needs to check. Very often, programmers write 'wrappers' of
existing functions that just check for the return value, as a way to decrease
the 'verbosity' of the source code, a well-known one is:
```c
void *xmalloc(size_t bytes)
{
    void *ptr;
    if ( (ptr = malloc(bytes)) == NULL )
    {
        fprintf(stderr, "Failed to allocate, aborting...\n");
        exit(EXIT_FAILURE);
    }
    return (ptr);
}
```
because if you *really* need that memory, it doesn't make sense to proceed.

## Features
Safemalloc addresses the case above, but also tries to go further. In addition
to checking for return values, safemalloc also features:

### Custom memory allocator
Just change the following sfmalloc.h settings to the routines of your choice:
```c
#define malloc_fn   malloc
#define calloc_fn   calloc
#define realloc_fn  realloc
#define free_fn     free
```

### Keeps track of _all_ memory allocations
Which means that there is **no undefined behavior**:
  + Calls to malloc and calloc with size 0 are treated as size 1 and returns a
    valid (able to be dereferenced) pointer.
  + Safe realloc: invalid pointers are handled properly and error behavior is
    configurable.
    + If error, a nice message like:
     ```
     [foo.c:6] main(): sf_realloc: invalid address: 0x9a4101!
     make sure that the address was previously allocated by [m,c,re]alloc!
     ```
     may appears on stderr (if `VERBOSE_MODE_2` or 3)

  + Safe free: just like `realloc`, invalid free pointers are also handled properly.
    + If error, a nice message like:
    ```
    [bar.c:10] main(): sf_free: invalid address: 0x684101!
    ```
    may appears on stderr (if `VERBOSE_MODE_2` or 3)
    + Double free is guaranteed not to happen =).

### Error behavior levels
Safemalloc provides 3 levels of verbose mode and 2 levels of 'decision' in case
of error, thus providing 6 different types of behavior, namely:
- *`VERBOSE_MODE_1`*: Suppress all safemalloc messages
- *`VERBOSE_MODE_2`*: Shows only error messages
- *`VERBOSE_MODE_3`*: Shows everything, error messages + usage data
- *`ON_ERROR_ABORT`*: If [m,c,re]alloc/free fails, abort the program
- *`ON_ERROR_NULL`* : If [m,c,re]alloc/free fails, returns NULL as usual, or
  silently returns (`sf_free()` case).

To select a behavior other than the default (`VERBOSE_MODE_2`, `ON_ERROR_ABORT`),
```c
int sf_init(int verbose_mode, int on_error);
```
needs to be explicitly invoked.

### No memory leaks
Since safemalloc maintains a list of all addresses allocated so far, safemalloc
will automatically (thanks to *atexit()*) deallocate all addresses when the
program ends.

In addition, if `VERBOSE_MODE_2` or `VERBOSE_MODE_3` is selected, a friendly
list will be displayed with all addresses not deallocated, as well as the file,
line number and function, something like:
```
Memory leaks were found during execution, please check below and fix.
----------------------------------------------------------------------
- Address 0x56d2940 (256 bytes) not freed!
  -> Allocated at [foo.c:100] main()

- Address 0x56764f0 (80 bytes) not freed!
  -> Allocated at [foo.c:70] do_bar()

- Address 0x5688020 (100 bytes) not freed!
  -> Allocated at [foo.c:89] something()

- Address 0x568acb0 (512 bytes) not freed!
  -> Allocated at [foo.c:110] main()
...
```
Please note that safemalloc **is not and does not intend to be** a garbage
collector!!, leaving safemalloc to deallocate all the memory in the end is a
bad practice and should be avoided.

Alternatively, safemalloc provides a `sf_free_all()` routine to deallocate all
memory so far, which can be useful to terminate the program given an error
condition.

### Handles cases of abrupt termination
Safemalloc registers signal handlers for `SIGINT` and `SIGTERM` automatically, which
means that memory leaks, memory release, etc. will continue to function even if
the program is terminated abruptly, or in cases of programs running in 'infinite
loop' (such as a server for example).

If your program already uses handlers for `SIGINT` and `SIGTERM`, you can change your
typical calls to signal for the equivalents of safemalloc: `sf_reg_sigint` and
`sf_reg_sigterm`, as in:

```c
void my_handler(int signal)
{
    if (signal == SIGINT)
        do_something();
    else
        do_other_something();
}

sf_reg_sigint(&my_handler);
sf_reg_sigterm(&myhandler);
```
and safemalloc will invoke your routines for you.

If you do not trust safemalloc, or do not want to use this feature, compile with
`-DSF_DISABLE_SIGNAL_HANDLERS`, and the use of signal handlers is fully disabled.

### Thread safe
Since safemalloc maintains a data structure of its own, concurrent access to data is a concern,
so safemalloc is thread-safe (or so it should be) and it is safe to be used in multi-threaded
environments.

### Drop-in replacement
Although more than a simple wrapper, safemalloc offers the same interface as
the standard libc [m,c,re]alloc/free functions, meaning that only 4 lines are
needed to switch the 'original' malloc for safemalloc in an existing project:
```c
#include "sfmalloc.h"
#define malloc  sf_malloc
#define calloc  sf_calloc
#define realloc sf_realloc
#define free    sf_free

int main()
{
    char *p = malloc(...);
    /* something. */
    free(p);
}
```
### Usage data
If `VERBOSE_MODE_3` is set, when the program ends, a small summary is displayed,
with simple information, such as total memory allocated, peak memory, number of
calls to malloc, calloc...

A code like below:
```c
#include "sfmalloc.h"
#define malloc sf_malloc
#define free   sf_free

int main()
{
    char *ptr;
    sf_init(VERBOSE_MODE_3, ON_ERROR_ABORT);

    ptr = malloc(128);
    for (int i = 0; i < 5; i++)
    {
        ptr = malloc(2*1024*1024);
        free(ptr);
    }
}
```
will produce the following output:
```
Memory leaks were found during execution, please check below and fix.
----------------------------------------------------------------------
- Address 0xab0100 (128 bytes) not freed!
  -> Allocated at [foo.c:11] main()

---------------------
     Usage data:
---------------------

Allocation infos:
-----------------
Total allocated (bytes): 10485888
Peak memory (bytes): 2097280
Leaked (bytes): 128

Calls info:
-----------
Malloc calls:  6
Calloc calls:  0
Realloc calls: 0
Free calls:    5
```
although simple, this output can be useful to get an idea of the program's
memory usage, without having to resort to external profilers and programs, ;-).

## Memory Consumption and Performance
All the benefits of safemalloc come at a cost: space and time. Safemalloc
internally uses a hash table to maintain all addresses and to be able to access
them at a low cost, however, the cost exists.

### Memory Consumption
Due to the usage of an auxiliary data structure, it is expected that there will
be an extra memory consumption, in addition to the allocated memory, for each
allocation.

However, since memory consumption is fixed, as the size per element increases,
the extra percentage of memory decreases. Also, as a measure to lower this
consumption, safemalloc offers a macro (`SFMALLOC_SAVE_SPACE`) which, when
defined, reduces the size of internal structures. As a result, informations
such as file name, function name and line number will not be stored and
displayed during a memory leak.

The graph below illustrates this scenario, allocating 100 elements at a time
where the size of each element varies from 50 to 1000 bytes.

<p align="center">
<img align="center" src="https://i.imgur.com/m1MtjPG.png"
alt="Memory Consumption per Element">
<br>
</p>

It can be observed, for example, that for 100 elements of 50 bytes, the memory
consumption is 73.6% higher than the ideal when `-DSFMALLOC_SAVE_SPACE` and
2.37x higher when not defined. For 100 bytes, memory consumption is 36.8% higher.

The following table highlights the main values:
| size_per_element | expected size (bytes) | size w/ SFMALLOC_SAVE_SPACE | size w/o SFMALLOC_SAVE_SPACE |
|------------------|-----------------------|-----------------------------|------------------------------|
| 50               | 5000                  | 8680 (73.6% bigger)         | 11888 (137.76% bigger)       |
| 100              | 10000                 | 13680 (36.8% bigger)        | 16888 (68.88% bigger)        |
| 200              | 20000                 | 23680 (18.4% bigger)        | 26888 (34.44% bigger)        |
| 500              | 50000                 | 53680 (7.36% bigger)        | 56888 (13.776% bigger)       |
| 1000             | 100000                | 103680 (3.68% bigger)       | 106888 (6.888% bigger)       |

Therefore, memory consumption needs to be considered before using this tool and
points like:
- What is the average size of the elements that I need to allocate?
- If small, will I allocate them very often?
- What is the average memory consumption the program is expected to have?

are important.

Safemalloc also provides a tool to simulate memory consumption accordingly to
the size of the elements and quantity, it can be found at benchs/space.

The chart above can be obtained with: `make benchs`. Note that the `R` package
is necessary in order to plot.

### Performance
In addition to the extra memory consumption, it is also expected that there will
be extra time consumption since it is a wrapper. In the chart below, 10k to 100k
allocations are made, from 256 bytes each on an i7 2600, running Ubuntu 18.04.3
x64, Linux v5.3.

<p align="center">
<img align="center" src="https://i.imgur.com/Cx4Hlic.png" alt="Time per Allocation">
<br>
</p>

It can be seen that, on average, sfmalloc is 1.5x to 2.33x slower than malloc,
but keep in mind that for 100,000 allocations, sfmalloc spent only ~ 33ms.
Unlike memory, I believe it is safe to say that the allocation time will not be
what will increase the execution time of your program.

The chart above can be obtained with: `make benchs`. Note that the `R` package
is necessary in order to plot.

## Building
Safemalloc requires only an ISO C99-compatible compiler, requires no
dependencies and consists of only two files: sfmalloc.c and sfmalloc.h. Just
add them to your project and include the header.

The available Makefiles compile the library's benchmarks and test files and can
be invoked with: `make benchs` and` make tests` respectively.

## License and Authors
Safemalloc is licensed under MIT License. Written by Davidson Francis and
(hopefully) others [contributors](https://github.com/Theldus/safemalloc/graphs/contributors).
