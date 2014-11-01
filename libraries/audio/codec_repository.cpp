#include "codec_repository.h"

#include "codecs/codec_registrar.h"
#include "codecs/mpadec_codec.h"
#ifdef HAS_SNDFILE
#include "codecs/sndfile_codec.h"
#endif

#include "data-access/data_access_factory.h"

#include <algorithm>
#include <mutex>

namespace invent
{
	namespace audio
	{
		namespace {
			std::once_flag registerFlag;
		}

		std::vector<RegisteredCodec> CodecRepository::sRegisteredCodecs;

		CodecRepository::CodecRepository()
		{
			registerCommonCodecs();
		}

		std::shared_ptr<IAudioCodec> CodecRepository::open(const std::string& aUrl)
		{
			auto access = DataAccessFactory::CreateForUrl(aUrl);

			for (auto codec : sRegisteredCodecs)
			{
				try
				{
					return codec.create(std::move(access));
				}
				catch (FormatNotSupportedException& aException)
				{
					continue;
				}
			}

			throw NoCodecForFiletype();
		}

		void CodecRepository::registerCodec(RegisteredCodec aCodecInfo)
		{
			sRegisteredCodecs.emplace_back(aCodecInfo);

			std::sort(sRegisteredCodecs.begin(), sRegisteredCodecs.end(), [](const RegisteredCodec& a, const RegisteredCodec& b) {
				return a.priority > b.priority;
			});
		}

		void CodecRepository::registerCommonCodecs()
		{
			std::call_once(registerFlag, doRegisterCommonCodecs);
		}

		void CodecRepository::doRegisterCommonCodecs()
		{
			CodecRegistrar<MPADECCodec>(0);
#ifdef HAS_SNDFILE
			CodecRegistrar<SndFileCodec>(10);
#endif
		}
	} // !namespace audio
} // !namespace invent
