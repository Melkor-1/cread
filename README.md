## Benchmarking Different Read Routines

### Routines:

* `getlines_mmap_memchr()`  - Memory-maps the file and replaces all newlines with null bytes whilst keeping track of the beginning of the lines. 
* `getlines_mmap_getline()` - Memory-maps the file, opens a stream on the mapped file with `fmemstream()`, and uses `getlines_getline()` to read from it.
* `getlines_getline()`      - Reads from a stream line by line with `getline()`.
* `getlines_fread`          - Reads from stream in chunks (64K-256K depending on the size of `size_t`).

### Benchmarks:

Generated 9.9 GB of random text data, spanning 20 files. The L: is # of lines and W: is # of words.

```none
F_01: L:65748 W:65000 112M
    1.323386s getline
    1.062281s mmap_getline
    1.359288s mmap_memchr
    1.223515s fread

F_02: L:927277 W:917225 1.1G
    10.295918s getline
    10.152524s mmap_getline
    10.703103s mmap_memchr
    9.441809s fread

F_03: L:994391 W:983497 1.7G
    15.532013s getline
    16.348079s mmap_getline
    16.402985s mmap_memchr
    15.530191s fread

F_04: L:144137 W:142530 116M
    1.174413s getline
    1.088987s mmap_getline
    1.229583s mmap_memchr
    1.039069s fread

F_05: L:212197 W:209866 390M
    3.775912s getline
    3.504295s mmap_getline
    3.705634s mmap_memchr
    3.168295s fread

F_06: L:232727 W:230108 357M
    3.563904s getline
    3.199386s mmap_getline
    3.540193s mmap_memchr
    2.906388s fread

F_07: L:89372 W:88356 65M
    0.650826s getline
    0.716422s mmap_getline
    0.652412s mmap_memchr
    0.558456s fread

F_08: L:569562 W:563291 550M
    5.172587s getline
    4.786773s mmap_getline
    5.275632s mmap_memchr
    4.394102s fread

F_09: L:453558 W:448606 675M
    6.748524s getline
    5.898616s mmap_getline
    6.250793s mmap_memchr
    5.098025s fread

F_10: L:138394 W:136879 243M
    2.375930s getline
    2.347826s mmap_getline
    2.392065s mmap_memchr
    2.041770s fread

F_11: L:290150 W:285186 22M
    0.332532s getline
    0.305136s mmap_getline
    0.240075s mmap_memchr
    0.256278s fread

F_12: L:318646 W:315055 527M
    4.764323s getline
    4.322212s mmap_getline
    4.791663s mmap_memchr
    4.178332s fread

F_13: L:576988 W:570725 1.1G
    10.798792s getline
    9.531844s mmap_getline
    8.840263s mmap_memchr
    9.837410s fread

F_14: L:783682 W:774984 855M
    7.521734s getline
    6.869232s mmap_getline
    7.205962s mmap_memchr
    7.217902s fread

F_15: L:814488 W:805405 806M
    7.151254s getline
    6.840751s mmap_getline
    6.978558s mmap_memchr
    6.740878s fread

F_16: L:249697 W:247063 423M
    3.899772s getline
    3.524430s mmap_getline
    3.844033s mmap_memchr
    3.334856s fread

F_17: L:586037 W:579086 236M
    2.499446s getline
    2.265058s mmap_getline
    2.266540s mmap_memchr
    2.048737s fread

F_18: L:407917 W:403336 329M
    3.203412s getline
    2.856143s mmap_getline
    3.083320s mmap_memchr
    2.693788s fread

F_19: L:592859 W:585774 222M
    2.398518s getline
    2.101303s mmap_getline
    2.161576s mmap_memchr
    2.010903s fread

F_20: L:233854 W:231300 292M
    2.832616s getline
    2.497263s mmap_getline
    2.726214s mmap_memchr
    2.588875s fread
```

[benchmark](benchmark.bash) was used to benchmark each routine. The caches were flushed before each run with:

```shell
sync && echo 3 >| /proc/sys/vm/drop_caches
```
