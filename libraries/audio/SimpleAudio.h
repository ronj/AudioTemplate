#ifndef SIMPLE_AUDIO_H
#define SIMPLE_AUDIO_H

#include <RtAudio.h>
#include <SamplerateControl.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

template <typename T>
class SimpleAudio
{
public:
  using sample_type = T;
  using sample_iterator = T*;
  using callback_type = std::function<void(sample_iterator,
                                           sample_iterator,
                                           sample_iterator,
                                           sample_iterator)>;

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
  explicit SimpleAudio(callback_type aCallback)
    : iCallback(aCallback)
    , iBufferSize(512)
    , iSamplerateControl(44100.0)
  {
    iInputParameters.deviceId = iDac.getDefaultInputDevice();
    iInputParameters.nChannels = 2;
    iOutputParameters.deviceId = iDac.getDefaultOutputDevice();
    iOutputParameters.nChannels = 2;

    //iStreamOptions.flags = RTAUDIO_MINIMIZE_LATENCY;
  }

  SimpleAudio()
    : SimpleAudio(nullptr)
  {
  }

  ~SimpleAudio()
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
                    &SimpleAudio::audioCallback,
                    static_cast<void*>(this),
                    &iStreamOptions);
  }

  void start()
  {
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

    SimpleAudio& audio = *static_cast<SimpleAudio*>(aData);
    unsigned int inputDataSize = aBufferFrameCount * audio.iInputParameters.nChannels;
    unsigned int outputDataSize = aBufferFrameCount * audio.iOutputParameters.nChannels;

    try
    {
      audio.iCallback(reinterpret_cast<sample_iterator>(aInputBuffer),
                      reinterpret_cast<sample_iterator>(aInputBuffer) + inputDataSize,
                      reinterpret_cast<sample_iterator>(aOutputBuffer),
                      reinterpret_cast<sample_iterator>(aOutputBuffer) + outputDataSize);
    }
    catch (std::bad_function_call& aException)
    {
      std::cout << aException.what() << ": You have not set an audio callback." << std::endl;
    }

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
void SimpleAudio<T>::open()
{
  throw std::logic_error(std::string("Sample type ") + typeid(T).name() + " not supported.");
}

template <>
void SimpleAudio<char>::open()
{
  doOpen(RTAUDIO_SINT8);
}

template <>
void SimpleAudio<signed short>::open()
{
  doOpen(RTAUDIO_SINT16);
}

template <>
void SimpleAudio<signed long>::open()
{
  doOpen(RTAUDIO_SINT32);
}

template <>
void SimpleAudio<float>::open()
{
  doOpen(RTAUDIO_FLOAT32);
}

template <>
void SimpleAudio<double>::open()
{
  doOpen(RTAUDIO_FLOAT64);
}

#endif // SIMPLE_AUDIO_H
