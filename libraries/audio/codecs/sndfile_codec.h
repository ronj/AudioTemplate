#ifndef SNDFILE_CODEC_H
#define SNDFILE_CODEC_H

#include "audio_codec.h"

#include <memory>
#include <string>
#include <vector>

class IAudioInfo;
class IDataAccess;

class SndFileCodec : public IAudioCodec
{
public:
	SndFileCodec(std::unique_ptr<IDataAccess> aDataAccess);
	virtual ~SndFileCodec() {}

	std::size_t decode(float* aSamples, std::size_t aSampleCount) const;
	std::size_t encode(const float* aSamples, std::size_t aSampleCount) const;

	const IAudioInfo& info() const;

	static std::vector<std::string> supportedExtensions();
	static std::vector<std::string> supportedMimeTypes();

private:
	class SndFileCodecImpl;
	std::shared_ptr<SndFileCodecImpl> iImpl;
};

#endif // SNDFILE_CODEC_H
