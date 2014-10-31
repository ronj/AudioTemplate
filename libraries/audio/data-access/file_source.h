#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#include "data_access.h"

#include <memory>
#include <functional>

class FileSource : public IDataAccess
{
public:
  FileSource(const std::string& aFilename) : iFilename(aFilename) {}
  bool open() { iFile = unique_ptr(std::fopen(iFilename.c_str(), "rb"), std::fclose); return true; }
  bool close() { iFile.reset(); return true; }
  bool seekable() const { return true; }

  std::size_t read(unsigned char* aBuffer, std::size_t aByteCount) { return std::fread(aBuffer, sizeof(unsigned char), aByteCount, iFile.get()); }
  std::size_t write(unsigned char* aBuffer, std::size_t aByteCount) { return std::fwrite(aBuffer, sizeof(unsigned char), aByteCount, iFile.get()); }
  std::size_t seek(long aByteOffset) { return std::fseek(iFile.get(), aByteOffset, SEEK_SET); }
  std::size_t offset() { return ::ftello(iFile.get()); }

private:
  using unique_ptr = std::unique_ptr<std::FILE, std::function<int(std::FILE *)>>;
  unique_ptr iFile;
  const std::string iFilename;
};

#endif // FILE_SOURCE_H
