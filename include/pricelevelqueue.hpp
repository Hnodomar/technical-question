#ifndef PRICE_LEVEL_QUEUE_HPP_
#define PRICE_LEVEL_QUEUE_HPP_

#include <list>
#include <cstdint>
#include <iostream>

namespace datafeed {

struct QueuePosition {
  QueuePosition(const uint64_t order_id, const uint32_t size) 
    : order_id(order_id), size(size)
  {}

  uint64_t order_id = 0;
  uint32_t size = 0;
};

using QueuePositionIt = std::list<QueuePosition>::iterator;

class PriceLevelQueue {
public:
  QueuePositionIt insert(const uint64_t order_id, const uint32_t size) {
    total_size_ += size;
    queue_.emplace_back(order_id, size);
    return std::prev(queue_.end());
  }
  void increase(QueuePositionIt& it, const uint32_t size) {
    total_size_ += size;
    it->size += size;
  }
  void decrease(QueuePositionIt& it, const uint32_t size) {
    if (total_size_ < size || it->size < size) {
      throw std::runtime_error("Queue decrease operation underflow");
    }
    total_size_ -= size;
    it->size -= size;
  }
  void erase(QueuePositionIt& it) {
    if (total_size_ < it->size) {
      throw std::runtime_error("Queue erase operation underflow");
    }
    total_size_ -= it->size;
    queue_.erase(it);
  }
  uint32_t getOrderDepth(const uint64_t order_id) const {
    uint32_t depth = 0;
    auto it = queue_.begin();    
    for (; it != queue_.end(); ++it) {
      if (it->order_id == order_id)
        return depth;
      depth += it->size;
    }

    if (it == queue_.end()) {
      throw std::runtime_error("Didn't find order ID in order depth query");
    }

    return 0;
  }
  uint32_t getTotalSize() const {
    return total_size_;
  }
private:

  uint32_t total_size_ = 0;
  std::list<QueuePosition> queue_;
};

}

#endif