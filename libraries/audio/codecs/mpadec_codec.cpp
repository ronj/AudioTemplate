#include "mpadec_codec.h"
#include "mpadec_info.h"

#include "data-access/data_access.h"

#include <mp3dec.h>

namespace virtual_io {
	size_t read(unsigned char* aData, std::size_t aSize, void* aUserData)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aUserData);
		return io->read(aData, aSize);
	}

	size_t seek(size_t aOffset, int aWhence, void* aUserData)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aUserData);
		if (!io->seekable())
			return -1;

		switch (aWhence) {
		case SEEK_SET:
			// offset remains unchanged
			break;
		case SEEK_CUR:
			aOffset += io->offset();
			break;
		case SEEK_END:
			//aOffset += io->length();
			break;
		}

		return io->seek(aOffset);
	}

	void close(void* aUserData)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aUserData);
		io->close();
	}
} // !namespace virtual_io

class MPADECCodec::Impl
{
public:
	Impl(std::unique_ptr<IDataAccess> aDataAccess)
		: iDecoder(mp3dec_init())
		, iDataAccess(std::move(aDataAccess))
	{
		if (!iDecoder)
		{
			throw std::bad_alloc();
		}

		iDataAccess->open();

		iConfig = { MPADEC_CONFIG_FULL_QUALITY, MPADEC_CONFIG_AUTO,
			MPADEC_CONFIG_FLOAT, MPADEC_CONFIG_LITTLE_ENDIAN,
			MPADEC_CONFIG_REPLAYGAIN_NONE, TRUE, TRUE, TRUE, 0.0 };

		mp3dec_virtual_io_t vio = { virtual_io::read, virtual_io::seek, virtual_io::close };
		apiWrapper(mp3dec_configure, iDecoder, &iConfig);
		apiWrapper(mp3dec_init_file, iDecoder, 0, FALSE, vio, iDataAccess.get());
		apiWrapper(mp3dec_get_info, iDecoder, iInfo.nativeHandle(), MPADEC_INFO_STREAM);
	}

	~Impl()
	{
		cleanup();
	}

	std::size_t decode(float* aSamples, std::size_t aSampleCount) const
	{
		unsigned int bufferUsedSize = 0;

		mp3dec_decode(iDecoder,
			reinterpret_cast<uint8_t*>(aSamples),
			aSampleCount * sizeof(float),
			&bufferUsedSize);

		return bufferUsedSize / sizeof(float);
	}

	std::size_t encode(const float*, std::size_t) const
	{
		throw std::logic_error("Not implemented");
	}

	const IAudioInfo& info() const
	{
		return iInfo;
	}

private:
	template <typename Method, typename... Args>
	void apiWrapper(Method&& method, Args... arguments)
	{
		int returnValue = MP3DEC_RETCODE_OK;
		if ((returnValue = method(arguments...)) != MP3DEC_RETCODE_OK)
		{
			cleanup();

			if (returnValue == MP3DEC_RETCODE_NOT_MPEG_STREAM)
			{
				throw FormatNotSupportedException();
			}

			throw std::logic_error(mp3dec_error(returnValue));
		}
	}

	void cleanup()
	{
		if (iDecoder)
		{
			mp3dec_uninit(iDecoder);
		}
	}

private:
	mp3dec_t iDecoder = nullptr;
	mpadec_config_t iConfig;
	MPADECInfo iInfo;
	std::unique_ptr<IDataAccess> iDataAccess;
};

MPADECCodec::MPADECCodec(std::unique_ptr<IDataAccess> aDataAccess)
	: iImpl(new Impl(std::move(aDataAccess)))
{
}

MPADECCodec::~MPADECCodec()
{
}

std::size_t MPADECCodec::decode(float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->decode(aSamples, aSampleCount);
}

std::size_t MPADECCodec::encode(const float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->encode(aSamples, aSampleCount);
}

const IAudioInfo& MPADECCodec::info() const
{
	return iImpl->info();
}

std::vector<std::string> MPADECCodec::supportedExtensions()
{
	return { "mp1", "mp2", "mp3" };
}

std::vector<std::string> MPADECCodec::supportedMimeTypes()
{
	return { "audio/mpeg", "audio/mpa", "audio/mpa-robust" };
}
