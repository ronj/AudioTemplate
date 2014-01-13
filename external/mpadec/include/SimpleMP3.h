#ifndef SIMPLE_MP3_H
#define SIMPLE_MP3_H

#include "mp3dec.h"

#include <vector>
#include <fcntl.h>
#include <unistd.h>

template <typename T>
class SimpleMP3
{
public:
  using sample_type = T;
  using decoded_data_container = std::vector<T>;

public:
  SimpleMP3(const std::string& aFilename, typename decoded_data_container::size_type aBufferSize)
    : iFile(open(aFilename.c_str(), O_RDONLY | O_BINARY))
    , iDecoder(mp3dec_init())
    , iDecodedData(aBufferSize)
  {
    if (!iDecoder)
    {
      throw std::bad_alloc();
    }

    apiWrapper(mp3dec_configure, iDecoder, &iConfig);
    apiWrapper(mp3dec_init_file, iDecoder, iFile, 0, FALSE);
    apiWrapper(mp3dec_get_info, iDecoder, &iInfo, MPADEC_INFO_STREAM);

    std::cout << "Opened file " << aFilename << std::endl;
    std::cout << "Details: Layer " << iInfo.layer
              << (iInfo.channels > 1 ? " stereo " : " mono ")
              << iInfo.bitrate << " kbps " << iInfo.frequency << " Hz "
              << iInfo.duration / 60 << ":" << iInfo.duration % 60 << std::endl;
  }

  ~SimpleMP3()
  {
    if (iDecoder)
    {
      mp3dec_uninit(iDecoder);
    }

    close(iFile);
  }

  unsigned int decode()
  {
    unsigned int bufferUsedSize = 0;

    mp3dec_decode(iDecoder,
    	            reinterpret_cast<unsigned char*>(&iDecodedData[0]),
    	            iDecodedData.capacity() * sizeof(T),
    	            &bufferUsedSize);

    return bufferUsedSize / sizeof(T);
  }

  const decoded_data_container& data() const
  {
  	return iDecodedData;
  }

private:
  template <typename Method, typename... Args>
  void apiWrapper(Method&& method, Args... arguments)
  {
    int returnValue = MP3DEC_RETCODE_OK;
    if ((returnValue = method(arguments...)) != MP3DEC_RETCODE_OK)
    {
      throw std::logic_error(mp3dec_error(returnValue));
    }
  }

private:
  int                    iFile = -1;
  mp3dec_t               iDecoder = nullptr;
  mpadec_config_t        iConfig = { MPADEC_CONFIG_FULL_QUALITY, MPADEC_CONFIG_AUTO,
                                     MPADEC_CONFIG_32BIT, MPADEC_CONFIG_LITTLE_ENDIAN,
                                     MPADEC_CONFIG_REPLAYGAIN_NONE, TRUE, TRUE, TRUE, 0.0 };
  mpadec_info_t          iInfo;
  decoded_data_container iDecodedData;
};

#endif // SIMPLE_MP3_H
