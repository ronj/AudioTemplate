#include "opus_info.h"

#include <opus/opusfile.h>

unsigned int OpusInfo::channels() const
{
	return iInfo->channel_count;
}

unsigned int OpusInfo::bitrate() const
{
	return op_bitrate(iFile, -1) / 1000;
}

unsigned int OpusInfo::samplerate() const
{
	return 48000;
}

IAudioInfo::audio_duration OpusInfo::duration() const
{
    return audio_duration(op_pcm_total(iFile, -1) / samplerate());
}

std::string OpusInfo::toString() const
{
    std::stringstream ss;

    ss << channelsToString() << " " << samplerateToString() << " "
       << bitrateToString() << " " << durationToString();

    return ss.str();
}

void OpusInfo::setNativeHandle(const OpusHead* aHeader) const
{
	iInfo = aHeader;
}

void OpusInfo::setNativeFileHandle(const OggOpusFile* aFile)
{
	iFile = aFile;
}
