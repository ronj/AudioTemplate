#include "SimpleAudio.h"

#include <algorithm>
#include <iostream>

using Audio = SimpleAudio<float>;

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  Audio audio([](Audio::sample_iterator aInBegin,
                 Audio::sample_iterator aInEnd,
                 Audio::sample_iterator aOutBegin,
                 Audio::sample_iterator aOutEnd)
               {
                 std::copy(aInBegin, aInEnd, aOutBegin);
               });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
