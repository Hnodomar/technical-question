#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "bookhandler.hpp"
#include "parser.hpp"
#include "csvreader.hpp"

TEST_CASE("Data Feed Basic Functionality") {

  CSVReader csv_reader("../../../tests/unit/dummydata.csv");

  datafeed::BookHandler book_handler;

  datafeed::parsing::CSVParser csv_parser(book_handler);

  while(!csv_reader.finishedReading()) {
    csv_parser.parseMessage(csv_reader.data_ptr);
  }

  SECTION("Add Order Best Prices") {
    
    const auto best_prices = book_handler.getTopOfBook();

    REQUIRE(best_prices.size() == 2);

    REQUIRE(best_prices[0].price == 802);
    REQUIRE(best_prices[0].depth == 298720);

    REQUIRE(best_prices[1].price == 801);
    REQUIRE(best_prices[1].depth == 1047760);

  }

  SECTION("Add Order Queue Position") {

    const uint32_t position = book_handler.getQueuePosition(7427602921427246994);

    REQUIRE(position == 1136230);

  }

  SECTION("Delete Order") {
    
    book_handler.deleteOrder(7427602921427245361);

    const uint32_t position = book_handler.getQueuePosition(7427602921427246994);

    REQUIRE(position == 1131230);

    const auto best_prices = book_handler.getTopOfBook();

    REQUIRE(best_prices.size() == 2);

    REQUIRE(best_prices[0].price == 802);
    REQUIRE(best_prices[0].depth == 298720);

    REQUIRE(best_prices[1].price == 801);
    REQUIRE(best_prices[1].depth == 1042760);

  }
/*
  SECTION("Execute Order") {

  }
  */

}

TEST_CASE("Data Feed Tests Whole CSV") {

  CSVReader csv_reader("../../../tests/unit/tickdata.csv");

  datafeed::BookHandler book_handler;

  datafeed::parsing::CSVParser csv_parser(book_handler);

  while(!csv_reader.finishedReading()) {
    csv_parser.parseMessage(csv_reader.data_ptr);
  }

  SECTION("Top of Book Correctness") {
    const auto best_prices = book_handler.getTopOfBook();
  }

  SECTION("Queue Position Correctness") {
    const auto position = book_handler.getQueuePosition(1);
  }
}