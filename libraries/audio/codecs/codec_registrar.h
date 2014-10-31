#ifndef CODEC_REGISTRAR_H
#define CODEC_REGISTRAR_H

#include "codec_repository.h"
#include "registered_codec.h"

#include <memory>

class IDataAccess;

namespace invent
{
	namespace audio
	{
		template <typename T>
		struct CodecRegistrar
		{
			explicit CodecRegistrar(int aPriority = 0)
			{
				RegisteredCodec codec;
				codec.supportedExtensions = T::supportedExtensions;
				codec.supportedMimeTypes = T::supportedMimeTypes;
				codec.create = CodecRegistrar::createCodec;
				codec.priority = aPriority;

				invent::audio::CodecRepository::registerCodec(std::move(codec));
			}

			static std::unique_ptr<IAudioCodec> createCodec(std::unique_ptr<IDataAccess> aSource)
			{
				return std::unique_ptr<T>(new T(std::move(aSource)));
			}
		};
	} // !namespace audio
} // !namespace invent

#endif // !CODEC_REGISTRAR_H
