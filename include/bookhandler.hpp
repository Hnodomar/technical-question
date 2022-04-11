#ifndef FEED_HANDLER_HPP_
#define FEED_HANDLER_HPP_

#include <iostream>
#include <boost/container/flat_map.hpp>
#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>

#include "oalphasmap.hpp"
#include "pricelevelqueue.hpp"

namespace datafeed {

struct OrderMetaData {
  OrderMetaData(const int16_t price, QueuePositionIt queue_position)
    : price(price), queue_position(queue_position)
  {}

  OrderMetaData () {}

  int16_t price = 0;
  QueuePositionIt queue_position;
};

struct BestPrice {
  BestPrice(const int16_t price, const uint32_t depth)
    : price(price), depth(depth)
  {}

  int16_t price;
  uint64_t depth;
};

using Price = int16_t;
using RBTreeQueueMap = std::map<Price, PriceLevelQueue, std::greater<int16_t>>;
using FlatQueueMap = boost::container::flat_map<Price, PriceLevelQueue, std::greater<int16_t>>;

template<typename QueueMapType>
class BookHandler {
public:

  BookHandler() : orders_(65536) {}

  void addOrder(const uint64_t order_id, const int16_t price, const uint32_t size, const uint16_t) {
    auto metadata_it = orders_.find(order_id);

    if (metadata_it == orders_.end()) {
      const auto queue_pos = book_[price].insert(order_id, size);
      orders_.emplace(order_id, price, queue_pos);
    }
    else {
      book_[price].increase(metadata_it->second.queue_position, size);
    }
  }
  
  void executeOrder(const uint64_t order_id, const uint32_t size) {
    auto it = orders_.find(order_id);

    if (it == orders_.end()) {
      throw std::runtime_error("Tried executing order with ID that doesn't exist");
    }

    auto queue_position = it->second.queue_position;
    auto& price_level = book_[it->second.price];

    if (size >= queue_position->size) {
      price_level.erase(queue_position);
      orders_.erase(it);
      return;
    }

    price_level.decrease(queue_position, size);
  }

  void deleteOrder(const uint64_t order_id) {
    auto it = orders_.find(order_id);

    if (it == orders_.end()) {
      throw std::runtime_error("Tried cancelling order with ID that doesn't exist");
    }
    
    const auto price = it->second.price;
    book_[price].erase(it->second.queue_position);

    orders_.erase(it);
  }
  
  using BestPrices = std::vector<BestPrice>;

  BestPrices getTopOfBook() const {
    std::vector<BestPrice> best_prices;

    int i = 0;  
    for (auto it = book_.begin(); i < 4 && it != book_.end(); ++it, ++i) {
      const auto& level = it->second;
      const auto price = it->first;
      const auto depth = level.getTotalSize();
      best_prices.emplace_back(price, depth);
    }

    return best_prices;
  }

  uint32_t getQueuePosition(const uint64_t order_id) {
    const auto order = orders_.find(order_id);

    if (order == orders_.end()) {
      throw std::runtime_error("Tried getting queue position of order that doesnt exist");
    }
  
    uint32_t size_ahead_of_order = 0;

    for (const auto& level : book_) {
      const auto& queue = level.second;
      const auto& price_level = level.first;
      if (price_level != order->second.price) {
        size_ahead_of_order += queue.getTotalSize();
      }
      else {
        size_ahead_of_order += queue.getOrderDepth(order_id);
      }
    }

    return size_ahead_of_order;
  }

  void clear() {
    book_.clear();
    OALPHashMap<uint64_t, OrderMetaData> temp(65536);
    orders_.swap(temp);
  }

private:
  
  using OrderID = uint64_t;
  OALPHashMap<OrderID, OrderMetaData> orders_;

  QueueMapType book_;
};

}

#endif