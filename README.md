# Ringer - Ring buffer for pointers.

Ringer is queue type storage for pointers (objects), essentially a
FIFO container. Storage is a continuous array. Ringer uses Read and
Write Indices for reading and writing, respectively.

Items (objects) are placed to Ringer with `put`. `put` advances the
Write Index, and at the end of the storage, it will wrap to start.

Items are retrieved from Ringer with `get`. `get` advances the Read
Index, and at the end of the storage, it will wrap to start.

For example before `put`, Ringer with size of 5 could be:

    0 ----
    1 4321 r
    2 3241
    3 3123
    4 ---- w

and after `put`:

    0 ---- w
    1 4321 r
    2 3241
    3 3123
    4 1111

Write and Read Indices have the same values when Ringer is empty and
full. Ringer maintains `count` in order to separate between the two
conditions.

Ringer struct:

    Field     Type          Addr
    ------------------------------
    ridx      (uint64_t)  | N + 0
    widx      (uint64_t)  | N + 8
    cnt       (uint64_t)  | N + 16
    size      (uint64_t)  | N + 24
    data[0]   (void*)     | N + 32

`ridx` is Read Index and defines the "front" (oldest item) of
Ringer. `widx` is Write Index and defines the "back" of Ringer. `cnt`
is the current item count within Ringer, and `size` defines the size
of the reserved storage as number of items.

`data` is an array used for storing the items. It can be used as fixed
size storage or it can be automatically resized (see below).

Base type for Ringer is `rg_t`. `rg_t` is a pointer to Ringer
struct. Some functions in Ringer library require a reference to
Ringer. Reference type is `rg_p` and it is a pointer to pointer of
Ringer struct. `rg_p` is required whenever there is possibility that
Ringer might get relocated in memory. This happens when more
reservation is needed for storage, or when Ringer is destroyed.

Ringer can be created to given size using `rg_new()`. Minimum size is
2.

    rg_t rg = rg_new( 16 );

Ringer can be destroyed with:

    rg_destroy( &rg );

`rg_destroy()` requires Ringer reference, since `rg` is set to `NULL`
after destroy.

Items can be added to the back of the container:

    void* data = obj;
    rg_put( &rg, data );

Items can be removed from front of the container:

    data = rg_get( rg );

When Ringer becomes full, `rg_put` fails. When Ringer is empty,
`rg_get` fails.

Ringer can be also used with automatic storage resizing. In order to
force a `put` operation:

    rg_ram( rg, data );

If Ringer is full when `rg_ram` is called, Ringer storage size will be
doubled, and the storing is performed as usual. Ringer data is shifted
to the start of the container, and Read and Write Indices are updated
accordingly.

Ringer does not automatically decrease storage size. This can be done
explicitly:

    rg_resize( &rg, 8 );

where 8 is the new size for Ringer storage. Operation fails if size is
too small. It can be either too small for Ringer (`RG_MIN_SIZE`) or
the current container data does not fit to the new size. `rg_resize`
can be also used to explicitly increase the container size.

There are functions that does not conform to normal queue type
ordering. There are `rg_put_front`, `rg_get_back`, `rg_peek_back`, and
`rg_get_nth` functions.

There are also query functions: `rg_count`, `rg_is_empty`,
`rg_is_full`, and `rg_size`.

Please refer to Doxygen documentation for details.


## Ringer API documentation

See Doxygen documentation. Documentation can be created with:

    shell> doxygen .doxygen


## Examples

All functions and their use is visible in tests. Please refer `test`
directory for testcases.


## Building

Ceedling based flow is in use:

    shell> ceedling

Testing:

    shell> ceedling test:all

User defines can be placed into `project.yml`. Please refer to
Ceedling documentation for details.


## Ceedling

Ringer uses Ceedling for building and testing. Standard Ceedling files
are not in GIT. These can be added by executing:

    shell> ceedling new ringer

in the directory above Ringer. Ceedling prompts for file
overwrites. You should answer NO in order to use the customized files.
