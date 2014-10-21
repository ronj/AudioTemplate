#ifndef INVENT_AUDIO_H
#define INVENT_AUDIO_H

#include "audio_device.h"
#include "codec_repository.h"

#include <common/readerwriterqueue.h>
#include <array>
#include <algorithm>
#include <thread>

namespace invent {
	namespace audio {
		using DefaultSampleType = float;
		using AudioIO = AudioDevice<DefaultSampleType>;

		enum class SampleLayout
		{
		  Interleaved,
		  NonInterleaved
		};

		template <unsigned int ChannelCount, SampleLayout Layout, typename SampleType, std::size_t BlockSize>
		class AudioBlock
		{
		public:
		  unsigned int channelCount() const
		  {
		    return ChannelCount;
		  }

		  std::size_t blockSize() const
		  {
		    return BlockSize;
		  }

		  SampleType* data()
		  {
		    return m_Data.data();
		  }

		  const SampleType* data() const
		  {
		    return m_Data.data();
		  }

		  std::size_t size() const
		  {
		    return m_Data.size();
		  }

		  typename std::array<SampleType, ChannelCount * BlockSize>::iterator begin() { return m_Data.begin(); }
		  typename std::array<SampleType, ChannelCount * BlockSize>::iterator end() { return m_Data.end(); }

		private:
		  std::array<SampleType, ChannelCount * BlockSize> m_Data;
		};

		using StereoInterleaved = AudioBlock<2, SampleLayout::Interleaved, DefaultSampleType, 512>;
		using MonoInterleaved = AudioBlock<1, SampleLayout::Interleaved, DefaultSampleType, 512>;

	    namespace blocks {
	      class DefaultDecoder
	      {
	      public:
	        DefaultDecoder(const IAudioCodec<DefaultSampleType>& aCodec)
	          : m_Codec(aCodec)
	          , m_Queue(128)
	          , m_SleepTime(static_cast<int>(std::round(((1.0 / m_Codec.info().samplerate()) * 1000) * 512)))
	        {
	          m_Thread = std::thread([this]()
	          {
	            std::size_t decodedSamples = 0;

	            do
	            {
	              StereoInterleaved decoderBuffer;
	              decodedSamples = m_Codec.decode(decoderBuffer.data(), decoderBuffer.size());

	              while (!m_Queue.try_enqueue(decoderBuffer))
	              {
	                std::this_thread::sleep_for(std::chrono::milliseconds(m_SleepTime));
	              }
	            } while (decodedSamples > 0);
	          });
	        }

	        void join()
	        {
	          m_Thread.join();
	        }

	        StereoInterleaved* peek()
	        {
	          return m_Queue.peek();
	        }

	        void pop()
	        {
	          m_Queue.pop();
	        }

	      private:
	        const IAudioCodec<DefaultSampleType>& m_Codec;
	        moodycamel::ReaderWriterQueue<StereoInterleaved> m_Queue;
	        std::thread m_Thread;
	        std::chrono::milliseconds m_SleepTime;
	      };
	    }
	}
}

#endif // INVENT_AUDIO_H
