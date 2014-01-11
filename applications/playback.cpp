#include "SimpleAudio.h"

#include "mp3dec.h"

#include <algorithm>
#include <iostream>

using Audio = SimpleAudio<signed long>;

#include <fcntl.h>
#include <unistd.h>
#include <vector>

class MP3Playback
{
public:
  explicit MP3Playback(std::string aFilename)
    : iFile(open(aFilename.c_str(), O_RDONLY | O_BINARY))
    , iDecoder(mp3dec_init())
    , iDecodedData(512)
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

  ~MP3Playback()
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

    mp3dec_decode(iDecoder, reinterpret_cast<unsigned char*>(&iDecodedData[0]), iDecodedData.capacity() * sizeof(Audio::sample_type), &bufferUsedSize);

    return bufferUsedSize;
  }

  void playback(Audio::sample_iterator aInBegin,
                Audio::sample_iterator aInEnd,
                Audio::sample_iterator aOutBegin,
                Audio::sample_iterator aOutEnd)
  {
    (void)aInBegin;
    (void)aInEnd;
    (void)aOutEnd;

    unsigned int decodedSamples = decode();

    if (decodedSamples > 0)
    {
      std::copy(iDecodedData.begin(), iDecodedData.begin() + decodedSamples, aOutBegin);
    }
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
  int      iFile = -1;
  mp3dec_t iDecoder = nullptr;
  mpadec_config_t iConfig = { MPADEC_CONFIG_FULL_QUALITY, MPADEC_CONFIG_AUTO,
                              MPADEC_CONFIG_32BIT, MPADEC_CONFIG_LITTLE_ENDIAN,
                              MPADEC_CONFIG_REPLAYGAIN_NONE, TRUE, TRUE, TRUE, 0.0 };
  mpadec_info_t iInfo;
  std::vector<Audio::sample_type> iDecodedData;
};

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.mp3" << std::endl;
    return -1;
  }

  MP3Playback mp3(argv[1]);
  Audio audio(Audio::binder(&MP3Playback::playback, mp3));
  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
