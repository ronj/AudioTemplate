#ifndef SOURCE_H
#define SOURCE_H

#include <common/noncopyable.h>

class ISource : public NonCopyable
{
public:
  virtual bool open() = 0;
  virtual bool close() = 0;
  virtual bool seekable() const = 0;

  virtual std::size_t read(unsigned char* aBuffer, std::size_t aByteCount) = 0;
  virtual std::size_t seek(std::size_t aByteOffset) = 0;
};

#endif // SOURCE_H
