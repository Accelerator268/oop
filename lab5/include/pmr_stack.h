#ifndef PMR_STACK_H
#define PMR_STACK_H

#include <memory_resource>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <stdexcept>

template<typename T>
class PMRStack {
private:
    struct Node {
        T data;
        Node* next;
        
        template<typename... Args>
        Node(Args&&... args)
            : data(std::forward<Args>(args)...), next(nullptr) {}
    };
    
    Node* top_;
    std::pmr::memory_resource* memory_resource_;
    size_t size_;
    
    //Вспомогательный метод для создания аллокатора
    std::pmr::polymorphic_allocator<Node> get_node_allocator() const {
        return std::pmr::polymorphic_allocator<Node>(memory_resource_);
    }
    
public:
    //Итератор
    class Iterator {
    private:
        Node* current_;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator(Node* node = nullptr) : current_(node) {}
        
        //Префиксный инкремент
        Iterator& operator++() {
            if (current_) {
                current_ = current_->next;
            }
            return *this;
        }
        
        //Постфиксный инкремент
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        //Разыменование
        reference operator*() const {
            return current_->data;
        }
        
        pointer operator->() const {
            return &current_->data;
        }
        
        //Сравнение
        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
        
        //Приведение к bool
        explicit operator bool() const {
            return current_ != nullptr;
        }
    };
    
    //Константный итератор
    class ConstIterator {
    private:
        const Node* current_;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        
        ConstIterator(const Node* node = nullptr) : current_(node) {}
        
        ConstIterator& operator++() {
            if (current_) {
                current_ = current_->next;
            }
            return *this;
        }
        
        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }
        
        reference operator*() const {
            return current_->data;
        }
        
        pointer operator->() const {
            return &current_->data;
        }
        
        bool operator==(const ConstIterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const ConstIterator& other) const {
            return !(*this == other);
        }
        
        explicit operator bool() const {
            return current_ != nullptr;
        }
    };
    
    //Конструкторы
    PMRStack(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : top_(nullptr), memory_resource_(mr), size_(0) {}
    
    PMRStack(std::initializer_list<T> init, 
             std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : top_(nullptr), memory_resource_(mr), size_(0) {
        for (auto it = init.end(); it != init.begin(); ) {
            --it;
            push(*it);
        }
    }
    
    //Правило пяти
    PMRStack(const PMRStack& other) 
        : top_(nullptr), memory_resource_(other.memory_resource_), size_(0) {
        if (!other.empty()) {
            //Копируем в обратном порядке
            PMRStack temp(other.memory_resource_);
            Node* current = other.top_;
            while (current != nullptr) {
                temp.push(current->data);
                current = current->next;
            }
            
            while (!temp.empty()) {
                push(temp.top());
                temp.pop();
            }
        }
    }
    
    PMRStack(PMRStack&& other) noexcept
        : top_(other.top_), memory_resource_(other.memory_resource_), 
          size_(other.size_) {
        other.top_ = nullptr;
        other.size_ = 0;
    }
    
    PMRStack& operator=(const PMRStack& other) {
        if (this != &other) {
            //Очищаем текущий стек
            clear();
            
            //Копируем memory_resource
            memory_resource_ = other.memory_resource_;
            
            //Копируем данные в обратном порядке
            if (!other.empty()) {
                PMRStack temp(other.memory_resource_);
                Node* current = other.top_;
                while (current != nullptr) {
                    temp.push(current->data);
                    current = current->next;
                }
                
                while (!temp.empty()) {
                    push(temp.top());
                    temp.pop();
                }
            }
        }
        return *this;
    }
    
    PMRStack& operator=(PMRStack&& other) noexcept {
        if (this != &other) {
            clear();
            top_ = other.top_;
            memory_resource_ = other.memory_resource_;
            size_ = other.size_;
            
            other.top_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    ~PMRStack() {
        clear();
    }
    
    //Основные операции
    void push(const T& value) {
        auto alloc = get_node_allocator();
        Node* new_node = alloc.allocate(1);
        try {
            alloc.construct(new_node, value);
            new_node->next = top_;
            top_ = new_node;
            ++size_;
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
    }
    
    void push(T&& value) {
        auto alloc = get_node_allocator();
        Node* new_node = alloc.allocate(1);
        try {
            alloc.construct(new_node, std::move(value));
            new_node->next = top_;
            top_ = new_node;
            ++size_;
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
    }
    
    template<typename... Args>
    void emplace(Args&&... args) {
        auto alloc = get_node_allocator();
        Node* new_node = alloc.allocate(1);
        try {
            alloc.construct(new_node, std::forward<Args>(args)...);
            new_node->next = top_;
            top_ = new_node;
            ++size_;
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
    }
    
    void pop() {
        if (!empty()) {
            Node* old_top = top_;
            top_ = top_->next;
            
            auto alloc = get_node_allocator();
            alloc.destroy(old_top);
            alloc.deallocate(old_top, 1);
            
            --size_;
        }
    }
    
    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return top_->data;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return top_->data;
    }
    
    //Информация
    bool empty() const { return top_ == nullptr; }
    size_t size() const { return size_; }
    
    //Очистка
    void clear() {
        while (!empty()) {
            pop();
        }
    }
    
    //Получение memory_resource
    std::pmr::memory_resource* get_memory_resource() const {
        return memory_resource_;
    }
    
    //Итераторы
    Iterator begin() { return Iterator(top_); }
    Iterator end() { return Iterator(nullptr); }
    
    ConstIterator begin() const { return ConstIterator(top_); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    
    ConstIterator cbegin() const { return ConstIterator(top_); }
    ConstIterator cend() const { return ConstIterator(nullptr); }
};

#endif //PMR_STACK_H