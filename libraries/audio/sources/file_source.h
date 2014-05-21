#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#include "source.h"

#include <memory>
#include <functional>

class FileSource : public ISource
{
public:
  FileSource(const std::string& aFilename) : iFilename(aFilename) {}
  bool open() { iFile = FilePtr(std::fopen(iFilename.c_str(), "r"), std::fclose); return true; }
  bool close() { iFile.reset(); return true; }
  bool seekable() const { return true; }

  std::size_t read(unsigned char* aBuffer, std::size_t aByteCount) { return ::fread(aBuffer, 1, aByteCount, iFile.get()); }
  std::size_t seek(std::size_t aByteOffset) { return ::fseeko(iFile.get(), aByteOffset, SEEK_SET); }

private:
  using FilePtr = std::unique_ptr<std::FILE, std::function<int(std::FILE *)>>;
  FilePtr iFile;
  const std::string iFilename;
};

#endif // FILE_SOURCE_H
