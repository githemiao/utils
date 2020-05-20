#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
// #include <chrono>
#include <functional>

struct Event {
    Event(int w) : what(w), userdata(nullptr) { }
    Event() : userdata(nullptr) {}

    int what;
    void *userdata;
};

typedef std::pair<Event, int64_t> EventItem;

class EventHandler {
public:
    EventHandler();
    virtual ~EventHandler();

    int exit();
    int finish();
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

    bool event_finish_;
    bool keep_running_;
    bool event_waiting_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread loop_thread_;
    std::list<EventItem> event_list_;
};

class UserHandler : public EventHandler {
public:
    UserHandler(std::function<void(const Event &evt)> fn) : handler_(fn) { }
    void handleEvent(const Event &evt) override { handler_(evt); }

private:
    std::function<void(const Event &evt)> handler_;
};

#endif //EVENT_HANDLER_H