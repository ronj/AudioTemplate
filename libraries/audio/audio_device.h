#ifndef AUDIO_IO_H
#define AUDIO_IO_H

#include <RtAudio.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <audio/samplerate_control.h>
#include <common/config.h>

template <typename T>
class AudioDevice
{
public:
  using sample_type = T;
  using sample_iterator = T*;
  using callback_type = std::function<void(sample_iterator,
                                           sample_iterator,
                                           sample_iterator,
                                           sample_iterator)>;

  static CFG_CONSTEXPR std::size_t DefaultBufferSize = 512;
  static CFG_CONSTEXPR int DefaultSamplerate = 44100;

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
  explicit AudioDevice(callback_type aCallback)
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

  AudioDevice()
    : AudioDevice(nullptr)
  {
  }

  ~AudioDevice()
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
                    &AudioDevice::audioCallback,
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

    AudioDevice& audio = *static_cast<AudioDevice*>(aData);
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
void AudioDevice<T>::open()
{
  throw std::logic_error(std::string("Sample type ") + typeid(T).name() + " not supported.");
}

template <>
void AudioDevice<char>::open()
{
  doOpen(RTAUDIO_SINT8);
}

template <>
void AudioDevice<signed short>::open()
{
  doOpen(RTAUDIO_SINT16);
}

template <>
void AudioDevice<signed long>::open()
{
  doOpen(RTAUDIO_SINT32);
}

template <>
void AudioDevice<float>::open()
{
  doOpen(RTAUDIO_FLOAT32);
}

template <>
void AudioDevice<double>::open()
{
  doOpen(RTAUDIO_FLOAT64);
}

#endif // AUDIO_IO_H
