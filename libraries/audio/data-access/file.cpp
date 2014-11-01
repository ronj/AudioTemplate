#include "file.h"

#include <memory>
#include <functional>

File::File(const std::string& aUrl)
  : iUrl(aUrl)
{
}

bool File::open()
{
  iFile = unique_ptr(std::fopen(iUrl.c_str(), "rb"), std::fclose);

  return true;
}

bool File::close()
{
  iFile.reset();

  return true;
}

bool File::seekable() const
{
  return true;
}

std::size_t File::read(unsigned char* aBuffer, std::size_t aByteCount)
{
  return std::fread(aBuffer, sizeof(unsigned char), aByteCount, iFile.get());
}

std::size_t File::write(unsigned char* aBuffer, std::size_t aByteCount)
{
  return std::fwrite(aBuffer, sizeof(unsigned char), aByteCount, iFile.get());
}

std::size_t File::seek(long aByteOffset)
{
  return std::fseek(iFile.get(), aByteOffset, SEEK_SET);
}

std::size_t File::offset()
{
  return ::ftello(iFile.get());
}
