#include "mpadec_info.h"

unsigned int MPADECInfo::layer() const
{
	return iInfo.layer;
}

unsigned int MPADECInfo::channels() const
{
	return iInfo.channels;
}

unsigned int MPADECInfo::bitrate() const
{
	return iInfo.bitrate;
}

unsigned int MPADECInfo::samplerate() const
{
	return iInfo.frequency;
}

IAudioInfo::audio_duration MPADECInfo::duration() const
{
	return audio_duration(iInfo.duration);
}

std::string MPADECInfo::toString() const
{
	std::stringstream ss;

	ss << mpegTypeString() << " " << layerToString() << " "
		<< channelsToString() << " " << samplerateToString() << " "
		<< bitrateToString() << " " << durationToString();

	return ss.str();
}

mpadec_info_t* MPADECInfo::nativeHandle()
{
	return &iInfo;
}

std::string MPADECInfo::mpegTypeString() const
{
	if (samplerate() < 16000)
	{
		return "MPEG-2.5";
	}
	else if (samplerate() < 32000)
	{
		return "MPEG-2";
	}

	return "MPEG-1";
}

std::string MPADECInfo::layerToString() const
{
	switch (layer())
	{
	case 1: return "Layer-I";
	case 2: return "Layer-II";
	case 3: return "Layer-III";
	default: return "Unknown Layer";
	}
}
