#ifndef INVENT_AUDIO_H
#define INVENT_AUDIO_H

#include "audio_device.h"
#include "codec_repository.h"

namespace invent {
	namespace audio {
		using DefaultSampleType = float;
		using AudioIO = AudioDevice<DefaultSampleType>;
	}
}

#endif INVENT_AUDIO_H
