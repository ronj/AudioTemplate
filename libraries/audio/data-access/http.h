#ifndef HTTP_DATA_ACCESS_H
#define HTTP_DATA_ACCESS_H

#include "data_access.h"

#include <memory>

class HTTP : public IDataAccess
{
public:
  HTTP(const std::string& aUrl);
  virtual ~HTTP();

  bool open();
  bool close();
  bool seekable() const;

  std::size_t read(unsigned char* aBuffer, std::size_t aByteCount);
  std::size_t write(unsigned char* aBuffer, std::size_t aByteCount);
  std::size_t seek(std::ptrdiff_t aByteOffset);
  std::ptrdiff_t offset();
  std::size_t size();

private:
  class Impl;
  std::unique_ptr<Impl> iImpl;
};

#endif // !HTTP_DATA_ACCESS_H
