#ifndef DATA_ACCESS_H
#define DATA_ACCESS_H

#include <common/noncopyable.h>

#include <cstddef>

class IDataAccess : public NonCopyable
{
public:
  virtual bool open() = 0;
  virtual bool close() = 0;

  virtual bool seekable() const = 0;

  virtual std::size_t read(unsigned char* aBuffer, std::size_t aByteCount) = 0;
  virtual std::size_t write(unsigned char* aBuffer, std::size_t aByteCount) = 0;
  virtual std::size_t seek(std::ptrdiff_t aByteOffset) = 0;
  virtual std::ptrdiff_t offset() = 0;
  virtual std::size_t size() = 0;
};

#endif // !DATA_ACCESS_H
