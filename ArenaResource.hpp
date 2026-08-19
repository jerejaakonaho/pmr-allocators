#pragma once
#include <memory_resource>
#include <cstddef>

class ArenaResource final : public std::pmr::memory_resource {
public:
    explicit ArenaResource(std::size_t size);
    ~ArenaResource() override;

    ArenaResource(const ArenaResource&) = delete;
    ArenaResource& operator=(const ArenaResource&) = delete;
    ArenaResource(ArenaResource&&) = delete;
    ArenaResource& operator=(ArenaResource&&) = delete;

private:
    std::byte* start_address_{nullptr};
    std::size_t offset_{};
    std::size_t total_size_{};

    // Override std::pmr::memory_resource virtual methods
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};
