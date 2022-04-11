#include <benchmark/benchmark.h>

#include <map>

#include "bookhandler.hpp"
#include "parser.hpp"
#include "csvreader.hpp"

template<typename BookHandler>
static inline void setupBook(BookHandler& book_handler) {
  CSVReader csv_reader("../../../tests/unit/tickdata.csv");
  datafeed::parsing::CSVParser<BookHandler> csv_parser(book_handler);
  while(!csv_reader.finishedReading()) {
    csv_parser.parseMessage(csv_reader.data_ptr);
  }
}

static void parseWholeCSVRBTree(benchmark::State& state) {
  for(auto _ : state) {
    namespace df = datafeed;
    using BookHandler = df::BookHandler<df::RBTreeQueueMap>;
    BookHandler book_handler;

    setupBook(book_handler);

    state.PauseTiming();
    book_handler.clear();
    state.ResumeTiming();
  }
}

static void parseWholeCSVFlatMap(benchmark::State& state) {
  for(auto _ : state) {
    namespace df = datafeed;
    using BookHandler = df::BookHandler<df::FlatQueueMap>;
    BookHandler book_handler;

    setupBook(book_handler);

    state.PauseTiming();
    book_handler.clear();
    state.ResumeTiming();
  }
}

static void addOrder(benchmark::State& state) {
  datafeed::BookHandler<datafeed::FlatQueueMap> book_handler;
  setupBook(book_handler);
  uint64_t id = 2;
  for (auto _ : state) {
    id += 1;
    book_handler.addOrder(id, 806, 10000, 0);
  }
  book_handler.clear();
}

static void deleteOrder(benchmark::State& state) {
  datafeed::BookHandler<datafeed::FlatQueueMap> book_handler;
  setupBook(book_handler);
  for (int i = 1; i < 25001; ++i) {
    book_handler.addOrder(i, 806, 5000, 0);
  }
  int id = 1;
  for (auto _ : state) {
    book_handler.deleteOrder(id);
    id += 1;
  }
  book_handler.clear();
}

static void executeOrder(benchmark::State& state) {

}

static void bestPricesFlatMap(benchmark::State& state) {
  datafeed::BookHandler<datafeed::FlatQueueMap> book_handler;
  setupBook(book_handler);
  for (auto _ : state) {
    const auto best_prices = book_handler.getTopOfBook();
  }
  book_handler.clear();
}

static void queuePositionFlatMap(benchmark::State& state) {
  datafeed::BookHandler<datafeed::FlatQueueMap> book_handler;
  setupBook(book_handler);
  for (auto _ : state) {
    const auto position = book_handler.getQueuePosition(7427602921438737869);
  }
  book_handler.clear();
}

static void bestPricesRBTree(benchmark::State& state) {
  datafeed::BookHandler<datafeed::RBTreeQueueMap> book_handler;
  setupBook(book_handler);
  for (auto _ : state) {
    const auto best_prices = book_handler.getTopOfBook();
  }
  book_handler.clear();
}

static void queuePositionRBTree(benchmark::State& state) {
  datafeed::BookHandler<datafeed::RBTreeQueueMap> book_handler;
  setupBook(book_handler);
  for (auto _ : state) {
    const auto position = book_handler.getQueuePosition(7427602921438737869);
  }
  book_handler.clear();
}

BENCHMARK(parseWholeCSVFlatMap);
BENCHMARK(parseWholeCSVRBTree);
BENCHMARK(addOrder)->Iterations(25000);
BENCHMARK(deleteOrder)->Iterations(25000);
BENCHMARK(bestPricesFlatMap);
BENCHMARK(bestPricesRBTree);
BENCHMARK(queuePositionFlatMap);
BENCHMARK(queuePositionRBTree);

BENCHMARK_MAIN();