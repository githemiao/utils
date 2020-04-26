#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

struct Event {
    Event(int w) : what(w), userdata(nullptr) { }
    Event() : userdata(nullptr) {}

    int what;
    void *userdata;
};

using std::chrono::steady_clock;
typedef std::pair<Event, int64_t> EventItem;

class EventHandler {
public:
    EventHandler();
    virtual ~EventHandler();

    int exit();
    int wait();
    int postEvent(const Event &evt, int64_t delayms = 0);
    virtual void handleEvent(const Event &evt) = 0;

private:
    void loop();

    struct Worker {
        Worker(EventHandler *h);
        void operator()();

        EventHandler *handler_;
    };
    friend struct Worker;

    bool keep_running_;
    bool event_waiting_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread loop_thread_;
    std::list<EventItem> event_list_;
};

template <typename T>
class UserHandler : public EventHandler {
public:
    UserHandler(T *p) : user(p) { }
    void handleEvent(const Event &evt) override { user->handleEvent(evt); }

private:
    T * user;
};

#endif //EVENT_HANDLER_H