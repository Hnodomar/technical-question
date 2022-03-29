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
  OrderMetaData(const int16_t price, QueuePosition* position, PriceLevelQueue* price_level)
    : price(price), queue_position(position), price_level(price_level)
  {}

  OrderMetaData () {}

  int16_t price = 0;
  QueuePosition* queue_position = nullptr;
  PriceLevelQueue* price_level = nullptr;
};

struct BestPrice {
  BestPrice(const int16_t price, const uint32_t depth)
    : price(price), depth(depth)
  {}

  int16_t price;
  uint64_t depth;
};

class BookHandler {
public:

  BookHandler() : orders_(65536) {}

  void addOrder(const uint64_t order_id, const int16_t price, const uint32_t size, const uint16_t queue_position) {

    auto metadata_it = orders_.find(order_id);

    if (metadata_it == orders_.end()) {
      
      uint16_t queue_pos = queue_position;

      if (book_.find(price) == book_.end()) {

        book_.emplace(price, 1);
        
      }

      for (auto& level : book_) {

        if (price != level.first) {

          queue_pos -= level.second.queue_size;

        }
        else {

          auto [position, price_level] = level.second.insert(order_id, size, queue_pos);

          orders_.emplace(order_id, price, position, price_level);

          return;

        }

      } 

      throw std::runtime_error("Couldn't insert order in book");

    }
    else {

      auto price_level = metadata_it->second.price_level;

      price_level->increase(metadata_it->second.queue_position, size);

    }

  }
  
  void executeOrder(const uint64_t order_id, const uint32_t size) {

    auto it = orders_.find(order_id);

    if (it == orders_.end()) {
      throw std::runtime_error("Tried executing order with ID that doesn't exist");
    }

    auto queue_position = it->second.queue_position;

    auto price_level = it->second.price_level;

    if (size >= queue_position->size) {

      price_level->erase(queue_position);

      orders_.erase(it);

      return;

    }

    price_level->reduce(queue_position, size);

  }

  void deleteOrder(const uint64_t order_id) {

    auto it = orders_.find(order_id);

    if (it == orders_.end()) {
      throw std::runtime_error("Tried cancelling order with ID that doesn't exist");
    }

    auto price_level = it->second.price_level;

    price_level->erase(it->second.queue_position);

    orders_.erase(it);

  }
  
  using BestPrices = std::vector<BestPrice>;

  BestPrices getTopOfBook() const {

    std::vector<BestPrice> best_prices;

    for (size_t i = 0; i < 4 && i < book_.size(); ++i) {

      const auto& level = (book_.begin() + i);

      const auto price = level->first;
      const auto depth = level->second.total_depth;

      best_prices.emplace_back(price, depth);
    }

    return best_prices;
  }

  uint32_t getQueuePosition(const uint64_t order_id) {

    const auto it = orders_.find(order_id);

    if (it == orders_.end()) {
      throw std::runtime_error("Tried getting queue position of order that doesnt exist");
    }
  
    uint32_t size_ahead_of_order = 0;

    for (const auto& level : book_) {

      if (level.first != it->second.price) {

        size_ahead_of_order += level.second.total_depth;
    
      }
      else {

        auto curr_node = level.second.head;

        while (curr_node->order_id != order_id) {

          size_ahead_of_order += curr_node->size;

          curr_node = curr_node->next;

          if (!curr_node) {
            throw std::runtime_error("Found nullptr before queue position");
          }

        }

      }

    }

    return size_ahead_of_order;
  }

private:
  
  using OrderID = uint64_t;
  OALPHashMap<OrderID, OrderMetaData> orders_;

  using Price = int16_t;
  boost::container::flat_map<Price, PriceLevelQueue, std::greater<int16_t>> book_;
};

}

#endif