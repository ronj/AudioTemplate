#include "opus_codec.h"
#include "opus_info.h"

#include "data-access/data_access.h"

namespace opus_virtual_io {
	int read(void* aSource, unsigned char* aData, int aSize)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aSource);
		return io->read(aData, aSize);
	}

	int seek(void* aSource, opus_int64 aOffset, int aWhence)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aSource);

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

	opus_int64 tell(void* aSource)
	{
		IDataAccess* io = static_cast<IDataAccess*>(aSource);
		return io->offset();
	}
} // !namespace opus_virtual_io

class OpusCodec::Impl
{
public:
	Impl(std::unique_ptr<IDataAccess> aDataAccess)
		: iDataAccess(std::move(aDataAccess))
		, iOpusFile(nullptr, nullptr)
	{
		iDataAccess->open();

		OpusFileCallbacks vio = {
			.read = opus_virtual_io::read,
			.seek = nullptr, //opus_virtual_io::seek,
			.tell = nullptr, //opus_virtual_io::tell,
			.close = nullptr
		};

		int error = 0;
		iOpusFile = std::unique_ptr<OggOpusFile, std::function<void(OggOpusFile*)>>(op_open_callbacks(iDataAccess.get(), &vio, nullptr, 0, &error), op_free);

		if (!iOpusFile)
		{
			if (error == OP_ENOTFORMAT)
			{
				throw FormatNotSupportedException();
			}

			throw std::logic_error("Could not create OpusFile instance. " + std::to_string(error));
		}

		iInfo.setNativeHandle(op_head(iOpusFile.get(), 0));
	}

	~Impl()
	{
	}

	std::size_t decode(float* aSamples, std::size_t aSampleCount) const
	{
		return op_read_float(iOpusFile.get(), aSamples, aSampleCount, nullptr);
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
	std::unique_ptr<IDataAccess> iDataAccess;
	std::unique_ptr<OggOpusFile, std::function<void(OggOpusFile*)>> iOpusFile;
	OpusInfo iInfo;
};

OpusCodec::OpusCodec(std::unique_ptr<IDataAccess> aDataAccess)
	: iImpl(new Impl(std::move(aDataAccess)))
{
}

OpusCodec::~OpusCodec()
{
}

std::size_t OpusCodec::decode(float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->decode(aSamples, aSampleCount);
}

std::size_t OpusCodec::encode(const float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->encode(aSamples, aSampleCount);
}

const IAudioInfo& OpusCodec::info() const
{
	return iImpl->info();
}

std::vector<std::string> OpusCodec::supportedExtensions()
{
	return { "opus" };
}

std::vector<std::string> OpusCodec::supportedMimeTypes()
{
	return { "audio/opus", "audio/ogg" };
}
