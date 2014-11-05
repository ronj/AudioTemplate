#include "data_access_factory.h"

#include "uri.h"

#include "file.h"
#include "http.h"

std::unique_ptr<IDataAccess> DataAccessFactory::CreateForUrl(const std::string& aUrl)
{
	URI uri(aUrl);

	if (uri.getScheme() == "file")
		return std::unique_ptr<IDataAccess>(new File(uri.getPath()));

#ifdef HAS_CURL
	if (uri.getScheme() == "http")
		return std::unique_ptr<IDataAccess>(new HTTP(uri.toString()));
#endif

	return std::unique_ptr<IDataAccess>(new File(aUrl));
}
