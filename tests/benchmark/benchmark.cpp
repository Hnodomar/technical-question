#include <benchmark/benchmark.h>

#include "bookhandler.hpp"
#include "parser.hpp"
#include "csvreader.hpp"

static void parseWholeCSV() {

  CSVReader csv_reader("../../../tests/unit/tickdata.csv");

  datafeed::BookHandler book_handler;

  datafeed::parsing::CSVParser csv_parser(book_handler);

  while(!csv_reader.finishedReading()) {
    csv_parser.parseMessage(csv_reader.data_ptr);
  }

}

static void benchmarkTest(benchmark::State& state) {
  for (auto _ : state) {
    parseWholeCSV();
  }
}

BENCHMARK(benchmarkTest);

BENCHMARK_MAIN();