#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include <sys/types.h>

#ifdef RESIZE_FBUF
    #include "cread/src/common.h"
    #include "cread/src/FileBuf.h"
    #define resize_fbuf              resize_fbuf_stub
    bool resize_fbuf_stub(FileBuf *) { return false; }
#endif  /* RESIZE_FBUF */

#ifdef ALLOC_AND_APPEND_LINE   
    #include "cread/src/common.h"
    #include "cread/src/FileBuf.h"
    #define alloc_and_append_line   alloc_and_append_line_stub
    bool alloc_and_append_line_stub(FileBuf *, size_t, char *) { return false; }
#endif /* ALLOC_AND_APPEND_LINE */

#ifdef APPEND_LINE             
    #include "cread/src/common.h"
    #include "cread/src/FileBuf.h"
    #define append_line             append_line_stub
    bool append_line_stub(FileBuf *, size_t, char *) { return false; }
#endif /* APPEND_LINE */

#ifdef FEOF                    
    #include <stdio.h>
    #define feof                    feof_stub
    int feof_stub(FILE *) { return 0; }
#endif /* FEOF */

#ifdef FILENO                  
    #include <stdio.h>
    #define fileno                  fileno_stub
    int fileno_stub(FILE *) { return -1; }
#endif /* FILENO */

#ifdef GETLINE                 
    #include <stdio.h>
    #define getline                 getline_stub
    ssize_t getline_stub(char **, size_t *, FILE *) { return -1; }
#endif /* GETLINE */

#ifdef FSTAT                   
    #include <sys/stat.h>
    #define fstat                   fstat_stub
    int fstat_stub(int, struct stat *) { return -1; }
#endif /* FSTAT */

#ifdef MMAP                    
    #include <sys/mman.h>
    #define mmap                    mmap_stub
    void *mmap_stub(void *, size_t, int, int, int, off_t ) { return MAP_FAILED; }
#endif /* MMAP */

#ifdef FMEMOPEN                
    #include <stdio.h>
    #define fmemopen                fmemopen_stub
    FILE *fmemopen_stub(void *, size_t, const char *) { return nullptr; }
#endif /* FMEMOPEN */

#ifdef READ                    
    #define read                    read_stub
    ssize_t read_stub(int, void *, size_t) { return -1; }
#endif /* READ */
