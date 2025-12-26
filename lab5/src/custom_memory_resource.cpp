#include "../include/custom_memory_resource.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <cstring>

CustomMemoryResource::CustomMemoryResource() {
    std::cout << "CustomMemoryResource created\n";
}

CustomMemoryResource::~CustomMemoryResource() {
    std::cout << "\nCustomMemoryResource destruction started...\n";
    std::cout << "Total blocks allocated during lifetime: " << blocks_.size() << "\n";
    
    //Проверяем, не осталось ли активных блоков
    size_t active = active_blocks_count();
    if (active > 0) {
        std::cerr << "WARNING: " << active << " blocks are still active!\n";
    }
    
    //Физически освобождаем все оставшиеся блоки
    for (auto& block : blocks_) {
        if (block.ptr && block.is_allocated) {
            std::cerr << "ERROR: Block " << block.ptr << " was not deallocated properly!\n";
            free(block.ptr);
        }
    }
    
    blocks_.clear();
    std::cout << "CustomMemoryResource destroyed\n";
}

void* CustomMemoryResource::do_allocate(size_t bytes, size_t alignment) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    //Сначала пытаемся найти подходящий освобожденный блок
    for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
        if (!it->is_allocated && it->size >= bytes) {
            //Проверяем выравнивание
            if (reinterpret_cast<uintptr_t>(it->ptr) % alignment == 0) {
                it->is_allocated = true;
                it->size = bytes;
                std::cout << "Reusing block: " << it->ptr 
                          << " size: " << bytes << " bytes\n";
                return it->ptr;
            }
        }
    }
    
    //Не нашли подходящий блок - выделяем новый
    void* ptr = nullptr;
    
    //Используем posix_memalign для Linux
    if (alignment < sizeof(void*)) {
        alignment = sizeof(void*);
    }
    if (posix_memalign(&ptr, alignment, bytes) != 0) {
        throw std::bad_alloc();
    }
    
    if (!ptr) {
        throw std::bad_alloc();
    }
    
    blocks_.emplace_back(ptr, bytes, true);
    std::cout << "Allocated new block: " << ptr 
              << " size: " << bytes << " bytes\n";
    
    return ptr;
}

void CustomMemoryResource::do_deallocate(void* p, size_t bytes, size_t alignment) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!p) return;
    
    // Находим блок и освобождаем его физически
    for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
        if (it->ptr == p) {
            if (it->is_allocated) {
                std::cout << "Deallocating block: " << p 
                          << " size: " << bytes << " bytes\n";
                
                //Вариант 1: Освобождаем физически и удаляем из списка
                free(p);
                
                blocks_.erase(it);
                return;
                
                //Вариант 2: Только помечаем как свободный (для пула)
                //it->is_allocated = false;
                //return;
            } else {
                std::cerr << "ERROR: Double free detected for block: " 
                          << p << std::endl;
                return;
            }
        }
    }
    
    //Если блок не найден в нашем пуле, освобождаем напрямую
    //(возможно, был выделен другим аллокатором)
    std::cout << "Deallocating external block: " << p << std::endl;
    free(p);
    
    (void)bytes;
    (void)alignment;
}

bool CustomMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

size_t CustomMemoryResource::allocated_blocks_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return blocks_.size();
}

size_t CustomMemoryResource::total_allocated_bytes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block.size;
    }
    return total;
}

size_t CustomMemoryResource::active_blocks_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::count_if(blocks_.begin(), blocks_.end(),
                         [](const BlockInfo& b) { return b.is_allocated; });
}

size_t CustomMemoryResource::active_bytes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = 0;
    for (const auto& block : blocks_) {
        if (block.is_allocated) {
            total += block.size;
        }
    }
    return total;
}

void CustomMemoryResource::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::cout << "Manual cleanup...\n";
    std::cout << "Total blocks: " << blocks_.size() 
              << ", Active blocks: " << active_blocks_count() << "\n";
    
    //Освобождаем только неактивные блоки
    for (auto it = blocks_.begin(); it != blocks_.end(); ) {
        if (!it->is_allocated) {
            if (it->ptr) {
                free(it->ptr);
            }
            it = blocks_.erase(it);
        } else {
            ++it;
        }
    }
    
    std::cout << "Cleanup completed. Remaining blocks: " << blocks_.size() << "\n";
}d. Remaining blocks: " << blocks_.size() << "\n";
}
