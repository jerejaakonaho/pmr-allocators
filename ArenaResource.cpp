#include "ArenaResource.hpp"
#include <new>
#include <sys/mman.h>
#include <cassert>
#include <memory> // std::align
/*
 * mmap    -> asking OS for contiguous memory
 * munmap  -> unmaps the contiguous memory (for destructor)
 * mlock   -> preventing the OS from moving the pages to the disk if idle (page fault prevention)
 * madvise -> use MADV_HUGEPAGE to get large pages for allocations, reducing TLB cache misses.
 */

ArenaResource::ArenaResource(std::size_t size) : total_size_(size) 
{
    assert(size > 0);

    void* start = mmap(nullptr, total_size_,
                      PROT_READ | PROT_WRITE, // Reads and writes allowed
                      MAP_PRIVATE | MAP_ANONYMOUS, // Private memory, for internal use only. Anonymous to prevent file I/O
                      -1, 0);
    
    if (start == MAP_FAILED) [[unlikely]] { // unlikely to allow compiler optimization
        throw std::bad_alloc{};
    }

    // cast void* to std::byte*
    start_address_ = static_cast<std::byte*>(start);
}

ArenaResource::~ArenaResource() {
    munmap(start_address_, total_size_);
}

void* ArenaResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    void* current_ptr = start_address_ + offset_;
    std::size_t space = total_size_ - offset_;

    void* aligned_ptr = std::align(alignment, bytes, current_ptr, space);

    if (aligned_ptr == nullptr) [[unlikely]] {
        throw std::bad_alloc{};
    }

    // Update offset
    std::size_t distance_from_start = static_cast<std::byte*>(aligned_ptr) - start_address_;
    offset_ = distance_from_start + bytes;

    return aligned_ptr;
}

void ArenaResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
    // left empty, arena allocators only clear memory only with the destructor (everything at once)
}

bool ArenaResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}
