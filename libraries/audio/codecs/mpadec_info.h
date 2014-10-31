#ifndef MPADEC_INFO_H
#define MPADEC_INFO_H

#include "audio_info.h"

#include <mpadec.h>

class MPADECInfo : public IAudioInfo
{
public:
	unsigned int layer() const;
	unsigned int channels() const;
	unsigned int bitrate() const;
	unsigned int samplerate() const;

	audio_duration duration() const;

	std::string toString() const;

	mpadec_info_t* nativeHandle();

private:
	std::string mpegTypeString() const;
	std::string layerToString() const;

private:
	mpadec_info_t iInfo;
};

#endif // MPADEC_INFO_H
