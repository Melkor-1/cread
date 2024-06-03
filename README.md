## Benchmarking Different Read Routines

### Routines:

* `readlines_mmap_memchr()`  - Memory-maps the file and replaces all newlines with null bytes whilst keeping track of the beginning of the lines. 
* `readlines_mmap_getline()` - Memory-maps the file, opens a stream on the mapped file with `fmemstream()`, and uses `readlines_getline()` to read from it.
* `readlines_getline()`      - Reads from a stream line by line with `getline()`.
* `readlines_fread()`        - Reads from stream in chunks (64K-256K depending on the size of `size_t`).
* `readlines_read()`         - Reads from file descriptor in chunks (64K-256K depending on the size of `size_t`).


## Environment:

[gen_files](gen_files) was used to generate the files (a script borrowed from
@CraigEstey on StackOverflow), and [benchmark](benchmark) 
was used to benchmark each routine. Due to the large size, the `/data` directory
that [benchmark](benchmark) expect is not present in this repository. It should be trivial to
replicate a similar (smaller or larger) data-set given [gen_files](gen_files). 

The caches were flushed before each run with:

```shell
sync && echo 3 >| /proc/sys/vm/drop_caches
```

The benchmarks were conducted on a Linux virtual machine. As per `neofetch`'s
output:

```none
CPU: Intel i5-8350U (2) @ 1.895GHz
Memory: 454MiB / 2933MiB
```

## Building

To build the main test program, run:

```bash
make debug  # Or make release 
```

To run the main program with all options under valgrind:

```bash
make valgrind
```

To generate the benchmarks:

```bash
make benchmark
```

Use [gen_files](gen_files) to generate the files in the `data` directory (that's
what the benchmarking script expects). 

The results are redirected to `"log.txt"`. Root priveleges may be required for this. 

To run the tests:

```bash
make test
```

## Benchmarks:

Generated 9.5 GB of random text data, spanning 16 files. The L: is # of lines and W: is # of words.

```none
F_01.txt: L:723938 W:715363 286M
    2.734618s fread
    3.214528s getline
    2.724019s mmap_getline
    2.793741s mmap_memchr
    2.382282s read

F_02.txt: L:1797557 W:1777578 1.6G
    15.836780s fread
    14.747851s getline
    14.493438s mmap_getline
    14.433059s mmap_memchr
    11.544710s read

F_03.txt: L:868508 W:858901 718M
    5.516691s fread
    6.325901s getline
    5.768159s mmap_getline
    6.288740s mmap_memchr
    5.345601s read

F_04.txt: L:1770447 W:1749818 748M
    5.940744s fread
    6.633095s getline
    6.086993s mmap_getline
    6.452443s mmap_memchr
    5.502765s read

F_05.txt: L:538590 W:532349 280M
    2.421899s fread
    2.652455s getline
    2.454733s mmap_getline
    2.602934s mmap_memchr
    2.267052s read

F_06.txt: L:1061222 W:1049762 1.9G
    18.299409s fread
    23.352706s getline
    16.143867s mmap_getline
    16.378157s mmap_memchr
    15.610494s read

F_07.txt: L:977623 W:966168 433M
    3.694574s fread
    4.209865s getline
    3.635462s mmap_getline
    4.034088s mmap_memchr
    3.563325s read

F_08.txt: L:1528488 W:1344242 7.3M
    0.465053s fread
    1.248155s getline
    0.895678s mmap_getline
    0.202998s mmap_memchr
    0.507976s read

F_09.txt: L:933222 W:920308 136M
    1.344580s fread
    1.685732s getline
    1.634934s mmap_getline
    1.433772s mmap_memchr
    1.316717s read

F_10.txt: L:1991643 W:1968250 810M
    6.613663s fread
    7.474445s getline
    6.827324s mmap_getline
    7.247532s mmap_memchr
    5.995010s read

F_11.txt: L:286725 W:283633 503M
    3.906051s fread
    4.549751s getline
    4.066533s mmap_getline
    4.473247s mmap_memchr
    3.707212s read

F_12.txt: L:469761 W:456504 13M
    0.211498s fread
    0.317302s getline
    0.302654s mmap_getline
    0.168478s mmap_memchr
    0.201805s read

F_13.txt: L:3239460 W:3195516 534M
    5.005339s fread
    5.715909s getline
    5.341514s mmap_getline
    5.111032s mmap_memchr
    4.526727s read

F_14.txt: L:3558066 W:3503292 352M
    3.539159s fread
    4.667701s getline
    4.273095s mmap_getline
    3.502702s mmap_memchr
    3.723208s read

F_15.txt: L:847341 W:837882 778M
    6.259525s fread
    6.900610s getline
    6.413894s mmap_getline
    6.945327s mmap_memchr
    5.730950s read

F_16.txt: L:1011765 W:1000220 581M
    4.905271s fread
    5.459073s getline
    4.972031s mmap_getline
    5.351933s mmap_memchr
    4.575030s read
```
