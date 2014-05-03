#ifndef AUDIO_IO_H
#define AUDIO_IO_H

#include <RtAudio.h>
#include <audio/samplerate_control.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

template <typename T>
class AudioIO
{
public:
  using sample_type = T;
  using sample_iterator = T*;
  using callback_type = std::function<void(sample_iterator,
                                           sample_iterator,
                                           sample_iterator,
                                           sample_iterator)>;

  static constexpr std::size_t DefaultBufferSize = 512;
  static constexpr float DefaultSamplerate = 44100.0f;

public:
  template <typename Method, typename Instance>
  static auto binder(Method&& m, Instance&& i) -> decltype(std::bind(m, &i,
                                                                     std::placeholders::_1,
                                                                     std::placeholders::_2,
                                                                     std::placeholders::_3,
                                                                     std::placeholders::_4))
  {
    using namespace std::placeholders;
    return std::bind(m, &i, _1, _2, _3, _4);
  }

public:
  explicit AudioIO(callback_type aCallback)
    : iCallback(aCallback)
    , iBufferSize(DefaultBufferSize)
    , iSamplerateControl(DefaultSamplerate)
  {
    iInputParameters.deviceId = iDac.getDefaultInputDevice();
    iInputParameters.nChannels = 2;
    iOutputParameters.deviceId = iDac.getDefaultOutputDevice();
    iOutputParameters.nChannels = 2;

    //iStreamOptions.flags = RTAUDIO_MINIMIZE_LATENCY;
  }

  AudioIO()
    : AudioIO(nullptr)
  {
  }

  ~AudioIO()
  {
    if (iDac.isStreamOpen())
    {
      iDac.closeStream();
    }
  }

  void play()
  {
    open();
    start();
  }

  void setCallback(callback_type aCallback)
  {
    iCallback = aCallback;
  }

  unsigned int bufferSize() const
  {
    return iBufferSize;
  }

  long int streamLatency()
  {
    return iDac.getStreamLatency();
  }

  SamplerateControl& samplerateControl()
  {
    return iSamplerateControl;
  }

private:
  void open();

  void doOpen(RtAudioFormat aAudioFormat)
  {
    iDac.openStream(&iOutputParameters,
                    &iInputParameters,
                    aAudioFormat,
                    iSamplerateControl.getSamplerate(),
                    &iBufferSize,
                    &AudioIO::audioCallback,
                    static_cast<void*>(this),
                    &iStreamOptions);
  }

  void start()
  {
    if (!iCallback)
    {
      throw std::logic_error("You are trying to start a stream without setting a callback.");
    }

    iDac.startStream();
  }

  static int audioCallback(void* aOutputBuffer,
                           void* aInputBuffer,
                           unsigned int aBufferFrameCount,
                           double aStreamTime,
                           RtAudioStreamStatus aStreamStatus,
                           void* aData)
  {
    (void)aStreamTime;
    (void)aStreamStatus;

    AudioIO& audio = *static_cast<AudioIO*>(aData);
    unsigned int inputDataSize = aBufferFrameCount * audio.iInputParameters.nChannels;
    unsigned int outputDataSize = aBufferFrameCount * audio.iOutputParameters.nChannels;

    audio.iCallback(reinterpret_cast<sample_iterator>(aInputBuffer),
                    reinterpret_cast<sample_iterator>(aInputBuffer) + inputDataSize,
                    reinterpret_cast<sample_iterator>(aOutputBuffer),
                    reinterpret_cast<sample_iterator>(aOutputBuffer) + outputDataSize);

    return 0;
  }

private:
  RtAudio                   iDac;
  callback_type             iCallback;
  unsigned int              iBufferSize;
  RtAudio::StreamParameters iInputParameters;
  RtAudio::StreamParameters iOutputParameters;
  RtAudio::StreamOptions    iStreamOptions;
  SamplerateControl         iSamplerateControl;
};

template <typename T>
void AudioIO<T>::open()
{
  throw std::logic_error(std::string("Sample type ") + typeid(T).name() + " not supported.");
}

template <>
void AudioIO<char>::open()
{
  doOpen(RTAUDIO_SINT8);
}

template <>
void AudioIO<signed short>::open()
{
  doOpen(RTAUDIO_SINT16);
}

template <>
void AudioIO<signed long>::open()
{
  doOpen(RTAUDIO_SINT32);
}

template <>
void AudioIO<float>::open()
{
  doOpen(RTAUDIO_FLOAT32);
}

template <>
void AudioIO<double>::open()
{
  doOpen(RTAUDIO_FLOAT64);
}

#endif // AUDIO_IO_H
