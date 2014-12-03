#include "opus_info.h"

unsigned int OpusInfo::channels() const
{
	return iInfo->channel_count;
}

unsigned int OpusInfo::bitrate() const
{
	return 0;
}

unsigned int OpusInfo::samplerate() const
{
	return 48000;
}

IAudioInfo::audio_duration OpusInfo::duration() const
{
    return audio_duration::zero();
}

std::string OpusInfo::toString() const
{
    std::stringstream ss;

    ss << channelsToString() << " " << samplerateToString() << " "
       << bitrateToString() << " " << durationToString();

    return ss.str();
}

void OpusInfo::setNativeHandle(const OpusHead* aHeader)
{
	iInfo = aHeader;
}
