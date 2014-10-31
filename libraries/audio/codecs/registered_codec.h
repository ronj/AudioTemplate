#ifndef REGISTERED_CODEC_H
#define REGISTERED_CODEC_H

#include <functional>
#include <string>
#include <memory>
#include <vector>

class IAudioCodec;
class IDataAccess;

struct RegisteredCodec
{
	using StringList = std::vector<std::string>;

	std::function<StringList(void)> supportedExtensions;
	std::function<StringList(void)> supportedMimeTypes;

	std::function<std::unique_ptr<IAudioCodec>(std::unique_ptr<IDataAccess>)> create;

	int priority;
};

#endif // !REGISTERED_CODEC_H
