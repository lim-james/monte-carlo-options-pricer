#include "pricer/concurrency/work_queue.h"
#include <mutex>


void WorkQueue::push(std::size_t batch) {
    std::lock_guard<std::mutex> lock{m_};
    queue_.push_back(batch);
}

bool WorkQueue::pop(std::size_t& out) {
    std::lock_guard<std::mutex> lock{m_};

    if (queue_.empty()) return false;
    
    out = queue_.front();
    queue_.pop_front();
    return true;
}


std::size_t WorkQueue::size() const {
    return queue_.size();
}
