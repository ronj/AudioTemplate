#include "data_access_factory.h"

#include "uri.h"

#include "file.h"
#include "http.h"

std::unique_ptr<IDataAccess> DataAccessFactory::CreateForUrl(const std::string& aUrl)
{
	HfURISyntax uri(aUrl);

	if (uri.getScheme() == "file")
		return std::unique_ptr<IDataAccess>(new File(uri.getPath()));

	if (uri.getScheme() == "http")
		return std::unique_ptr<IDataAccess>(new HTTP(uri.toString()));

	return std::unique_ptr<IDataAccess>(new File(aUrl));;
}
