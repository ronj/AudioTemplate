#ifndef MPADEC_CODEC_H
#define MPADEC_CODEC_H

#include "audio_codec.h"

#include <memory>
#include <string>
#include <vector>

class IAudioInfo;
class IDataAccess;

class MPADECCodec : public IAudioCodec
{
public:
	MPADECCodec(std::unique_ptr<IDataAccess> aDataAccess);
	virtual ~MPADECCodec() {}

	std::size_t decode(float* aSamples, std::size_t aSampleCount) const;
	std::size_t encode(const float* aSamples, std::size_t aSampleCount) const;

	const IAudioInfo& info() const;

	static std::vector<std::string> supportedExtensions();
	static std::vector<std::string> supportedMimeTypes();

private:
	class MPADECCodecImpl;
	std::shared_ptr<MPADECCodecImpl> iImpl;
};

#endif // MPADEC_CODEC_H
