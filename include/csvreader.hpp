#ifndef CSV_READER_HPP_
#define CSV_READER_HPP_

#include <boost/iostreams/device/mapped_file.hpp>
#include <cstdint>

struct CSVReader {
  CSVReader(const std::string& filename) 
    : data(filename)
    , data_ptr(reinterpret_cast<const uint8_t*>(data.data())) 
    , data_end_ptr(data_ptr + data.size())
  {}

  bool finishedReading() const {
    return data_ptr >= data_end_ptr;
  }

  boost::iostreams::mapped_file_source data;
  const uint8_t* data_ptr;
  const uint8_t* data_end_ptr;
};

#endif