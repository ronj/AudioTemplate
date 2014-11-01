#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#include "data_access.h"

#include <memory>
#include <string>

class File : public IDataAccess
{
public:
  File(const std::string& aUrl);

  bool open();
  bool close();
  bool seekable() const;

  std::size_t read(unsigned char* aBuffer, std::size_t aByteCount);
  std::size_t write(unsigned char* aBuffer, std::size_t aByteCount);
  std::size_t seek(long aByteOffset);
  std::size_t offset();

private:
  using unique_ptr = std::unique_ptr<std::FILE, std::function<int(std::FILE *)>>;
  unique_ptr iFile;
  const std::string iUrl;
};

#endif // FILE_SOURCE_H
