#include "ArenaResource.hpp"
#include <new>
#include <sys/mman.h>
/*
 * mmap    -> asking OS for contiguous memory
 * munmap  -> unmaps the contiguous memory (for destructor)
 * mlock   -> preventing the OS from moving the pages to the disk if idle (page fault prevention)
 * madvise -> use MADV_HUGEPAGE to get large pages for allocations, reducing TLB cache misses.
 */
