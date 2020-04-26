#ifndef OBJECT_QUEUE_H
#define OBJECT_QUEUE_H

#include <list>
#include <chrono>
#include <mutex>
#include <condition_variable>

template <typename T>
class ObjectQueue {
public:
    ObjectQueue();
    ~ObjectQueue();

    int push(T *obj);

    T *pop(int timeout = -1);

    int size();

private:
    std::mutex mutex_;
    std::condition_variable cond_;
    std::list<T *> object_list_;
};

template<typename T>
inline ObjectQueue<T>::ObjectQueue() {
}

template<typename T>
inline ObjectQueue<T>::~ObjectQueue() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.notify_all();
}

template<typename T>
inline int ObjectQueue<T>::push(T *obj) {
    std::unique_lock<std::mutex> lock(mutex_);

    object_list_.emplace_back(obj);
    cond_.notify_one();

    return 0;
}

template<typename T>
inline T *ObjectQueue<T>::pop(int timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (object_list_.empty()) {
        if (timeout > 0) {
                cond_.wait_for(lock, std::chrono::milliseconds(timeout));
        } else if (timeout < 0) {
            cond_.wait(lock);
        }
    }

    T *obj = nullptr;
    if (!object_list_.empty()) {
        obj = object_list_.front();
        object_list_.pop_front();
    }

    return obj;
}

template<typename T>
inline int ObjectQueue<T>::size() {
    return object_list_.size();
}

#endif