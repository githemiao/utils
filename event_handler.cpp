#include "event_handler.h"
#include "now.h"

#include <chrono>

typedef std::unique_lock<std::mutex> Autolock;
using std::chrono::milliseconds;
using std::chrono::microseconds;

EventHandler::EventHandler() :
    event_waiting_(false)
{
    keep_running_ = true;
    event_finish_ = false;
    loop_thread_ = std::thread(Worker(this));
}

EventHandler::~EventHandler()
{
    exit();
    wait();
}

int EventHandler::exit()
{
    keep_running_ = false;
    cond_.notify_all();

    return 0;
}

int EventHandler::finish() {
    event_finish_ = true;
    return 0;
}

int EventHandler::wait() {
    if (loop_thread_.joinable()) {
        loop_thread_.join();
    }

    return 0;
}

static bool event_compare(const EventItem &t1, const EventItem &t2)
{
    if (t1.second < t2.second) {
        return true;
    }

    return false;
}

int EventHandler::postEvent(const Event &evt, int64_t delayms)
{
    Autolock lock(mutex_);

    if (event_finish_) {
        return -1;
    }

    auto timeout = now_in_msec(STEADY_CLOCK) + delayms;
    event_list_.push_back({ evt, timeout });
    event_list_.sort(event_compare);
    if (event_waiting_) {
        cond_.notify_one();
    }
    return 0;
}

void EventHandler::loop()
{
    while (keep_running_) {
        std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);

        lock.lock();
        if (event_list_.empty()) {
            if (event_finish_) {
                keep_running_ = false;
            } else {
                event_waiting_ = true;
                cond_.wait(lock);
            }
        }

        if (!event_list_.empty()) {
            const EventItem &item = event_list_.front();
            auto now = now_in_msec(STEADY_CLOCK);
            if (item.second <= now) {
                Event evt = item.first;
                event_list_.pop_front();

                lock.unlock();
                handleEvent(evt);
                lock.lock();
            } else {
                long diff = item.second - now;
                cond_.wait_for(lock, milliseconds(diff));
            }
        }
        lock.unlock();
    }
}

EventHandler::Worker::Worker(EventHandler *h) :
    handler_(h)
{
}

void EventHandler::Worker::operator()()
{
    if (handler_) {
        handler_->loop();
    }
}
