#include "musepack_info.h"

unsigned int MPCDECInfo::channels() const
{
	return iInfo.channels;
}

unsigned int MPCDECInfo::bitrate() const
{
	return iInfo.bitrate;
}

unsigned int MPCDECInfo::samplerate() const
{
	return iInfo.sample_freq;
}

IAudioInfo::audio_duration MPCDECInfo::duration() const
{
	return audio_duration(mpc_streaminfo_get_length(&iInfo));
}

std::string MPCDECInfo::toString() const
{
	std::stringstream ss;

	ss << channelsToString() << " " << samplerateToString() << " "
	   << bitrateToString() << " " << durationToString();

	return ss.str();
}

mpc_streaminfo* MPCDECInfo::nativeHandle()
{
	return &iInfo;
}
