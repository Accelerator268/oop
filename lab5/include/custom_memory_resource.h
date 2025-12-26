#ifndef CUSTOM_MEMORY_RESOURCE_H
#define CUSTOM_MEMORY_RESOURCE_H

#include <memory_resource>
#include <vector>
#include <cstddef>
#include <mutex>

struct BlockInfo {
    void* ptr;
    size_t size;
    bool is_allocated;
    
    BlockInfo(void* p, size_t s, bool allocated = true)
        : ptr(p), size(s), is_allocated(allocated) {}
};

class CustomMemoryResource : public std::pmr::memory_resource {
private:
    std::vector<BlockInfo> blocks_;
    mutable std::mutex mutex_;
    
protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* p, size_t bytes, size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
    
public:
    CustomMemoryResource();
    ~CustomMemoryResource();
    
    //Статистика
    size_t allocated_blocks_count() const;
    size_t total_allocated_bytes() const;
    size_t active_blocks_count() const;
    size_t active_bytes() const;
    
    //Очистка только неактивных блоков
    void cleanup();
    
    CustomMemoryResource(const CustomMemoryResource&) = delete;
    CustomMemoryResource& operator=(const CustomMemoryResource&) = delete;
};

#endif //CUSTOM_MEMORY_RESOURCE_H