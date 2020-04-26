#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <list>
#include <mutex>

class DefaultMemoryAllocator
{
public:
    static inline void *alloc(size_t size) {
        return ::operator new (size, ::std::nothrow);
    }

    static inline void free(void *pointer, size_t size) {
        ::operator delete(pointer);
    }
};

template <typename T, typename MemoryAllocator = DefaultMemoryAllocator>
class ObjectPool
{
public:
    ObjectPool(int capacity = 30);
    ~ObjectPool();

    T *create();
    void release(T *object);

    void setCapacity(int capacity);
    int capacity();
    int size();

private:
    T *allocCachingObject();
    T *allocNewObject();

private:
    int capacity_;
    std::list<T *> working_objects_;
    std::list<T *> caching_objects_;
    std::mutex mutex_;
};

template<typename T, typename MemoryAllocator>
inline ObjectPool<T, MemoryAllocator>::ObjectPool(int capacity) : capacity_(capacity){
}

template<typename T, typename MemoryAllocator>
inline ObjectPool<T, MemoryAllocator>::~ObjectPool() {
    std::unique_lock<std::mutex> lock(mutex_);

    for (T *obj : working_objects_) {
        MemoryAllocator::free(obj, sizeof(T));
    }

    for (T *obj : caching_objects_) {
        MemoryAllocator::free(obj, sizeof(T));
    }

    working_objects_.clear();
    caching_objects_.clear();
}

template<typename T, typename MemoryAllocator>
inline T *ObjectPool<T, MemoryAllocator>::create() {
    if (!caching_objects_.empty()) {
        return allocCachingObject();
    } else {
        if (size() < capacity_) {
            return allocNewObject();
        }
    }

    return nullptr;
}

template<typename T, typename MemoryAllocator>
inline void ObjectPool<T, MemoryAllocator>::release(T *object) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto it = std::find(working_objects_.begin(), working_objects_.end(), object);
    caching_objects_.splice(caching_objects_.end(), working_objects_, it);
}

template<typename T, typename MemoryAllocator>
inline void ObjectPool<T, MemoryAllocator>::setCapacity(int capacity) {
    capacity_ = capacity;
}

template<typename T, typename MemoryAllocator>
inline int ObjectPool<T, MemoryAllocator>::capacity() {
    return capacity_;
}

template<typename T, typename MemoryAllocator>
inline int ObjectPool<T, MemoryAllocator>::size() {
    return working_objects_.size() + caching_objects_.size();
}

template<typename T, typename MemoryAllocator>
inline T *ObjectPool<T, MemoryAllocator>::allocCachingObject() {
    std::unique_lock<std::mutex> lock(mutex_);
    working_objects_.splice(working_objects_.end(), caching_objects_, caching_objects_.begin());

    return working_objects_.back();
}

template<typename T, typename MemoryAllocator>
inline T *ObjectPool<T, MemoryAllocator>::allocNewObject() {
    std::unique_lock<std::mutex> lock(mutex_);

    T *object = (T *)MemoryAllocator::alloc(sizeof(T));
    working_objects_.emplace_back(object);
    return object;
}

#endif