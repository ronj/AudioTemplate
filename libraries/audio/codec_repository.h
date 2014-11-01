#ifndef CODEC_REPOSITORY_H
#define CODEC_REPOSITORY_H

#include "codecs/registered_codec.h"

#include <exception>
#include <memory>
#include <vector>

class IAudioCodec;

namespace invent
{
	namespace audio
	{
		class NoCodecForFiletype : public std::exception
		{
			virtual const char* what() const throw()
			{
				return "Format is not supported by any of the registered codecs.";
			}
		};

		class CodecRepository
		{
		public:
			CodecRepository();

			static std::shared_ptr<IAudioCodec> open(const std::string& aUrl);

			static void registerCodec(RegisteredCodec aCodecInfo);
			static void registerCommonCodecs();

		private:
			static void doRegisterCommonCodecs();

		private:
			static std::vector<RegisteredCodec> sRegisteredCodecs;
		};
	} // !namespace audio
} // !namespace invent

#endif // !CODEC_REPOSITORY_H
