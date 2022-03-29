#ifndef FEED_PARSER_HPP_
#define FEED_PARSER_HPP_

#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <string>

#include "bookhandler.hpp"

namespace datafeed {
namespace parsing {

// timestamp will be 20 bytes long in CSV format for the next 300 years. +1 for skipping the ','
constexpr uint8_t MESSAGE_TYPE_OFFSET = 20;
constexpr uint8_t MESSAGE_PRICE_OFFSET = MESSAGE_TYPE_OFFSET + 2;

// for why I use strtol, please see: https://tinodidriksen.com/2010/02/cpp-convert-string-to-int-speed/

class CSVParser {
public:

  CSVParser(BookHandler& handler) : handler_(handler) {}

  void parseMessage(const uint8_t*& msg) {

    const uint64_t timestamp = parseCSVLong(msg);

    const uint8_t message_type = msg[0];

    msg += 2;

    switch(message_type) {
      case 'A':
        return addOrder(msg);
      case 'E':
        return executeOrder(msg);
      case 'D':
        return deleteOrder(msg);
      default:
        throw std::runtime_error("Parsed message with unknown type");
    }
  }

private:

  void addOrder(const uint8_t*& msg) {

    const int16_t price = parseCSVPrice(msg);

    const uint16_t q_position = parseCSVShort(msg);

    const uint32_t size = parseCSVLong(msg);

    const uint64_t order_id = parseCSVLong(msg);

    msg += 1;

    handler_.addOrder(order_id, price, size, q_position);

  }

  void executeOrder(const uint8_t*& msg) {

    skipNullField(msg);
    skipNullField(msg);
    
    const uint16_t size = parseCSVShort(msg);
    
    const uint64_t order_id = parseCSVLong(msg);

    msg += 1;

    handler_.executeOrder(order_id, size);

  }

  void deleteOrder(const uint8_t*& msg) {

    skipNullField(msg);
    skipNullField(msg);
    skipNullField(msg);

    const uint64_t order_id = parseCSVLong(msg);

    msg += 1;

    handler_.deleteOrder(order_id);

  }

  uint16_t parseCSVShort(const uint8_t*& msg) const {

    const char* field_begin = reinterpret_cast<const char*>(msg);
    char* field_end;

    errno = 0;

    const uint16_t field = strtol(field_begin, &field_end, 10);

    if (errno) {
      throw std::runtime_error("Failed to convert CSV field to integer");
    }

    msg = reinterpret_cast<const uint8_t*>(field_end) + 1;

    return field;

  }

  uint64_t parseCSVLong(const uint8_t*& msg) const {
    const char* field_begin = reinterpret_cast<const char*>(msg);
    char* field_end;

    errno = 0;

    const uint64_t field = strtoll(field_begin, &field_end, 10);

    if (errno) {
      throw std::runtime_error("Failed to convert CSV field to integer");
    }

    msg = reinterpret_cast<const uint8_t*>(field_end) + 1;

    return field;
  }

  int16_t parseCSVPrice(const uint8_t*& msg) const {

    const char* field_begin = reinterpret_cast<const char*>(msg);
    char* field_end;

    errno = 0;

    const int16_t price = strtof(field_begin, &field_end) * 100;

    if (errno) {
      throw std::runtime_error("Failed to convert CSV field to integer");
    }

    msg = reinterpret_cast<const uint8_t*>(field_end) + 1;

    return price;

  }
  
  void skipNullField(const uint8_t*& msg) const {
    msg += 2;
  }

  BookHandler& handler_;
};

}
}

#endif