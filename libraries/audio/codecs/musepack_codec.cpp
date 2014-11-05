#include "musepack_codec.h"
#include "musepack_info.h"

#include "data-access/data_access.h"

#include <cassert>
#include <mpc/mpcdec.h>

#include <iostream>

namespace virtual_io {
	mpc_int32_t read(mpc_reader* aReader, void* aData, mpc_int32_t aSize)
	{
		assert(aReader);
		std::cout << "read" << std::endl;
		IDataAccess* io = static_cast<IDataAccess*>(aReader->data);
		return io->read(static_cast<unsigned char*>(aData), aSize);
	}

	mpc_bool_t seek(mpc_reader* aReader, mpc_int32_t aOffset)
	{
		assert(aReader);
		std::cout << "seek" << std::endl;
		IDataAccess* io = static_cast<IDataAccess*>(aReader->data);
		return io->seek(aOffset);
	}

	mpc_int32_t tell(mpc_reader* aReader)
	{
		assert(aReader);
		std::cout << "tell" << std::endl;
		IDataAccess* io = static_cast<IDataAccess*>(aReader->data);
		return io->offset();
	}

	mpc_int32_t size(mpc_reader* aReader)
	{
		assert(aReader);
		std::cout << "size" << std::endl;
		IDataAccess* io = static_cast<IDataAccess*>(aReader->data);
		return -1;
	}

	mpc_bool_t canseek(mpc_reader* aReader)
	{
		assert(aReader);
		std::cout << "canseek" << std::endl;
		IDataAccess* io = static_cast<IDataAccess*>(aReader->data);
		return io->seekable();
	}
} // !namespace virtual_io

class MPCDECCodec::Impl
{
public:
	Impl(std::unique_ptr<IDataAccess> aDataAccess)
		: iDataAccess(std::move(aDataAccess))
		, iReader({ virtual_io::read, virtual_io::seek, virtual_io::tell, virtual_io::size, virtual_io::canseek, iDataAccess.get() })
		, iDemux(nullptr)
	{
		iDataAccess->open();

		iDemux = mpc_demux_init(&iReader);

		if (iDemux == nullptr) throw std::logic_error("Could not init demuxer");
		//mpc_demux_get_info(iDemux, iInfo.nativeHandle());
	}

	~Impl()
	{
		if (iDemux)
		{
			mpc_demux_exit(iDemux);
			iDemux = nullptr;
		}
	}

	std::size_t decode(float* aSamples, std::size_t aSampleCount) const
	{
		MPC_SAMPLE_FORMAT buffer[MPC_DECODER_BUFFER_LENGTH];
		unsigned int samplesRead = 0;

		while (samplesRead < aSampleCount)
		{
			mpc_frame_info frame;
			frame.buffer = buffer;

			mpc_status status = mpc_demux_decode(iDemux, &frame);

			samplesRead += frame.samples;

			if (frame.bits == -1)
				break;
		}

		return samplesRead;
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
	mpc_reader iReader;
	mpc_demux* iDemux;
	MPCDECInfo iInfo;
};

MPCDECCodec::MPCDECCodec(std::unique_ptr<IDataAccess> aDataAccess)
	: iImpl(new Impl(std::move(aDataAccess)))
{
}

MPCDECCodec::~MPCDECCodec()
{
}

std::size_t MPCDECCodec::decode(float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->decode(aSamples, aSampleCount);
}

std::size_t MPCDECCodec::encode(const float* aSamples, std::size_t aSampleCount) const
{
	return iImpl->encode(aSamples, aSampleCount);
}

const IAudioInfo& MPCDECCodec::info() const
{
	return iImpl->info();
}

std::vector<std::string> MPCDECCodec::supportedExtensions()
{
	return { "mpc" };
}

std::vector<std::string> MPCDECCodec::supportedMimeTypes()
{
	return { "audio/x-musepack", "audio/x-mpc", "audio/musepack", "audio/mpc" };
}
