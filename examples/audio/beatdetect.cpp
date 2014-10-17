#include <audio/audio.h>
#include <dsp/analysis/fft.h>
#include <dsp/analysis/beat_detector.h>

#include <algorithm>
#include <iostream>
#include <chrono>
#include <iostream>

using Audio = invent::audio::AudioIO;

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  std::shared_ptr<Fft> fft = Fft::create();
  BeatDetektor beat_lo(48, 95);
  BeatDetektor beat_mid(85, 169, &beat_lo);
  BeatDetektor beat_high(150, 280, &beat_lo);
  std::vector<float> signal(512);
  std::vector<float> amplitude(fft->getBinSize());
  auto start = std::chrono::steady_clock::now();

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
               {
                 std::copy(aInBegin, aInEnd, signal.begin());
                 fft->setSignal(signal);

                 std::copy(fft->getAmplitude(), fft->getAmplitude() + fft->getBinSize(), amplitude.begin());

                 float timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
                 beat_lo.process(timestamp / 1000.f, amplitude);
                 beat_mid.process(timestamp / 1000.f, amplitude);
                 beat_high.process(timestamp / 1000.f, amplitude);

                 //std::cout << "Time: " << time_in_sec << " Current BPM: " << beat.current_bpm << " Winning BPM: " << beat.win_bpm_int << std::endl;
               });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
