#include <audio/audio.h>
#include <dsp/generators/oscillator.h>
#include <dsp/generators/waveforms.h>
#include <dsp/generators/noise.h>

#include <array>
#include <algorithm>
#include <iostream>

using Audio = invent::audio::AudioIO;

float mtof(int midiNote) {
  return 440.0 * std::exp(0.057762265 * (midiNote - 69.0));
}

template <typename RandomGenerator = std::default_random_engine>
struct random_selector
{
  //On most platforms, you probably want to use std::random_device("/dev/urandom")()
  random_selector(RandomGenerator g = RandomGenerator(std::random_device()()))
    : gen(g) {}
 
  template <typename Iter>
  Iter select(Iter start, Iter end) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(gen));
    return start;
  }
 
  //convenience function
  template <typename Iter>
  Iter operator()(Iter start, Iter end) {
    return select(start, end);
  }
 
  //convenience function that works on anything with a sensible begin() and end(), and returns with a ref to the value type
  template <typename Container>
  auto operator()(const Container& c) -> decltype(*begin(c))& {
    return *select(begin(c), end(c));
  }
 
private:
  RandomGenerator gen;
};

class Decay
{
public:
  Decay(float aDuration)
    : iDecay(0.999 + aDuration * 0.0009)
  {
  }

  float operator()(unsigned int aTrigger)
  {
    if(aTrigger == 1)
    {
      iValue = 1;
    }
    else
    {
      iValue *= iDecay;
    }

    return iValue;
  }

private:
  float iDecay;
  float iValue = 0.0f;
};

class Impulse
{
public:
  Impulse(float aPeriod, float aSamperate = 44100.0f, float aPhase = 0.0f)
    : period(aPeriod * aSamperate)
    , phase(aPhase)
    , pos(period * aPhase)
  {
  }

  unsigned int operator()()
  {
    if (pos == 0)
    {
      pos++;
      return 1;
    }

    pos++;
    if (pos > period) pos = 0;

    return 0;
  }

private:
  float period;
  float phase;
  float pos;
};


int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  unsigned int bpm = 120;

  std::array<int, 10> notes = { { 40, 41, 28, 28, 28, 28, 27, 25, 35, 78 } };
  std::array<int, 10> sweepRate = { { 1, 6, 6, 2, 1, 2, 4, 8, 3, 3 } };
  std::array<int, 16> kickSequence = { { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0 } };
  unsigned int sweepPos = 0;
  unsigned int kickPos = 0;

  Oscillator<WaveformType::Saw<float>> sawA(0.0);
  Oscillator<WaveformType::Saw<float>> sawB(0.0);
  Oscillator<WaveformType::Triangle<float>> sweep(0.0);
  Oscillator<WaveformType::Sine<float>> kick(0.0);
  WhiteNoise<> noise;

  Decay snareDecayA(0.4);
  Decay snareDecayB(0.97);
  Decay kickDecay(0.85);

  Impulse trigger(bpm / 120.0f);
  Impulse snareTrigger(bpm / 50.0f, 44100.0f, 0.5);
  Impulse kickTrigger(bpm / 480.0f);

  random_selector<> randomSelect;
  float targetFrequency = 0.0f;
  float bassFrequency = 0.0f;

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
                {
                  for (Audio::sample_iterator it = aOutBegin; it != aOutEnd; it += 2)
                  {
                    if (trigger())
                    {
                      targetFrequency = mtof(randomSelect(notes));
                      sweep.setFrequency(sweepRate[sweepPos++]);
                      if (sweepPos >= sweepRate.size()) sweepPos = 0;
                    }

                    bassFrequency = targetFrequency * 0.0008 + bassFrequency * 0.9992;
                    sawA.setFrequency(bassFrequency * 1.01);
                    sawB.setFrequency(bassFrequency * 0.99);

                    int kickTrig = kickTrigger();
                    if (kickTrig)
                    {
                      kickTrig &= kickSequence[kickPos++];
                      if (kickPos >= kickSequence.size()) kickPos = 0;
                    }

                    float kickDec = kickDecay(kickTrig);
                    kick.setFrequency(40.0 + (kickDec * kickDec * kickDec * 200));

                    int snareTrig = snareTrigger();
                    float snareEnvelope = snareDecayA(snareTrig) * 2 + snareDecayB(snareTrig) * 0.05;

                    float snareLeft = noise() * snareEnvelope;
                    float snareRight = noise() * snareEnvelope;
                    float kickSample = kick() * kickDec * 7;
                    float sample = sawA() + sawB() + kickSample;

                    *it = sample + snareLeft;
                    *(it + 1) = sample + snareRight;
                  }
                });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
