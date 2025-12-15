#pragma once

#include <deque>
#include <mutex>

class WorkQueue {
private:

    std::deque<std::size_t> queue_;
    std::mutex m_;

public:

    void push(std::size_t batch);
    bool pop(std::size_t& out);

    std::size_t size() const;

};
