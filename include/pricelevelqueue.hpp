#ifndef PRICE_LEVEL_QUEUE_HPP_
#define PRICE_LEVEL_QUEUE_HPP_

#include <cstdint>
#include <utility>

namespace datafeed {

struct QueuePosition {

  QueuePosition(const uint64_t order_id, const uint32_t size, QueuePosition* next, QueuePosition* prev) 
    : next(next)
    , prev(prev)
    , order_id(order_id)
    , size(size)
  {}


  QueuePosition* next = nullptr;
  QueuePosition* prev = nullptr;

  uint64_t order_id = 0;
  uint32_t size = 0;
};

struct PriceLevelQueue {

  PriceLevelQueue(const int a) {
    head = new QueuePosition(0, 0, nullptr, nullptr);
    tail = new QueuePosition(0, 0, nullptr, nullptr);
    head->next = tail;
    tail->prev = head;
  }


  ~PriceLevelQueue() {

    auto curr_node = head;

    while (curr_node) {

      auto temp = curr_node;

      curr_node = curr_node->next;

      delete temp;

    }

  }

  void increase(QueuePosition* node, const uint32_t size) {

    node->size += size;

    total_depth += size;

  }

  void reduce(QueuePosition* node, const uint32_t size) {

    node->size -= size;

    if (size > total_depth) {
      throw std::runtime_error("Overflowed in queue size reduce operation");
    }

    total_depth -= size;

  }

  void erase(QueuePosition* node) {

    if (!node) {
      throw std::runtime_error("Tried erasing node that is null");
    }

    auto behind = node->prev;
    auto in_front = node->next;

    behind->next = in_front;
    in_front->prev = behind;

    --queue_size;

    total_depth -= node->size;

    delete node;

  }
  
  using InsertMetaData = std::pair<QueuePosition*, PriceLevelQueue*>;

  InsertMetaData insert(const uint64_t order_id, const uint32_t size, const uint16_t position) {

    ++queue_size;

    total_depth += size;

    if (position == 1) { // O(1)
      return insertAtHead(order_id, size);
    }
    else if (position >= queue_size) { // O(1)
      return insertAtTail(order_id, size);
    }
    else {  // O(N)
      return insertAtMiddle(order_id, size, position);
    }

  }

  InsertMetaData insertAtHead(const uint64_t order_id, const uint32_t size) {

    QueuePosition* queue_pos = new QueuePosition(order_id, size, nullptr, nullptr);

    queue_pos->prev = head;
    queue_pos->next = head->next;

    auto in_front = head->next;
    in_front->prev = queue_pos;

    head->next = queue_pos;

    return {queue_pos, this};

  }

  InsertMetaData insertAtTail(const uint64_t order_id, const uint32_t size) {

    QueuePosition* queue_pos = new QueuePosition(order_id, size, nullptr, nullptr);

    queue_pos->next = tail;
    queue_pos->prev = tail->prev;

    auto behind = tail->prev;
    behind->next = queue_pos;

    tail->prev = queue_pos;

    return {queue_pos, this};

  }

  InsertMetaData insertAtMiddle(const uint64_t order_id, const uint32_t size, const uint16_t position) {

    std::cout << "asdasddas" << std::endl;

    QueuePosition* curr_node = head;

    for (uint16_t i = 1; i < position; ++i) {
      
      if (!curr_node) {
        throw std::runtime_error("Tried placing order at queue position that doesn't exist");
      }

      curr_node = curr_node->next;

    }

    std::cout << std::to_string(queue_size) + " " + std::to_string(position) << std::endl;

    QueuePosition* temp = curr_node->next;

    QueuePosition* queue_pos = new QueuePosition(order_id, size, curr_node->next, curr_node);

    curr_node->next = queue_pos;

    if (temp) {
      
      temp->prev = curr_node->next;

    }

    return {queue_pos, this};

  }

  uint64_t total_depth = 0;
  uint16_t queue_size = 0;

  QueuePosition* head = nullptr;
  QueuePosition* tail = nullptr;
};

}

#endif