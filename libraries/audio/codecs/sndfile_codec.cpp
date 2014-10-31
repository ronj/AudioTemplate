#include "sndfile_codec.h"
#include "sndfile_info.h"

#include "data-access/data_access.h"

#include <sndfile.h>

#include <iostream>

namespace virtual_io {
	sf_count_t read(void* aData, sf_count_t aSize, void* aUserData)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aUserData);
		return io->read(static_cast<unsigned char*>(aData), aSize);
	}

	sf_count_t seek(sf_count_t aOffset, int aWhence, void* aUserData)
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

	sf_count_t tell(void* aUserData)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aUserData);
		return io->offset();
	}

	sf_count_t length(void* aUserData)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aUserData);
		return -1;
	}
}

class SndFileCodec::SndFileCodecImpl
{
public:
	using unique_ptr = std::unique_ptr<SNDFILE, int(*)(SNDFILE *)>;

	SndFileCodecImpl(std::unique_ptr<IDataAccess> aDataAccess)
		: iSndFile(nullptr, nullptr)
		, iDataAccess(std::move(aDataAccess))
	{
		iDataAccess->open();

		SF_VIRTUAL_IO vio = { virtual_io::length, virtual_io::seek, virtual_io::read, nullptr, virtual_io::tell };
		iSndFile = unique_ptr(sf_open_virtual(&vio, SFM_READ, iInfo.nativeHandle(), iDataAccess.get()), sf_close);

		if (sf_error(iSndFile.get()))
		{
			throw FormatNotSupportedException();
		}
	}

	std::size_t decode(float* aSamples, std::size_t aSampleCount) const
	{
		return sf_read_float(iSndFile.get(), aSamples, aSampleCount);
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
	unique_ptr iSndFile;
	SndFileInfo iInfo;
	std::unique_ptr<IDataAccess> iDataAccess;
};

SndFileCodec::SndFileCodec(std::unique_ptr<IDataAccess> aDataAccess)
	: iImpl(std::make_shared<SndFileCodecImpl>(std::move(aDataAccess)))
{
}

std::size_t SndFileCodec::decode(float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->decode(aSamples, aSampleCount);
}

std::size_t SndFileCodec::encode(const float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->encode(aSamples, aSampleCount);
}

const IAudioInfo& SndFileCodec::info() const
{
	return iImpl->info();
}

std::vector<std::string> SndFileCodec::supportedExtensions()
{
	int majorCount = 0;
	sf_command(nullptr, SFC_GET_FORMAT_MAJOR_COUNT, &majorCount, sizeof(int));
	std::vector<std::string> formats(majorCount);

	for (int i = 0; i < majorCount; ++i)
	{
		SF_FORMAT_INFO formatInfo;
		formatInfo.format = i;
		if (0 == sf_command(nullptr, SFC_GET_FORMAT_MAJOR, &formatInfo, sizeof(formatInfo)))
		{
			std::string extension = formatInfo.extension;
			formats.push_back(extension);
		}
	}

	return formats;
}

std::vector<std::string> SndFileCodec::supportedMimeTypes()
{
	return { "" };
}
