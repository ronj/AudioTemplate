#include "data_access_factory.h"

#include "uri.h"

#include "file.h"
#include "http.h"

#include <iostream>

std::unique_ptr<IDataAccess> DataAccessFactory::CreateForUrl(const std::string& aUrl)
{
	HfURISyntax uri(aUrl);

	std::cout << "Scheme: " << uri.getScheme() << std::endl;

	if (uri.getScheme() == "file")
		return std::unique_ptr<IDataAccess>(new File(uri.getPath()));

	if (uri.getScheme() == "http")
		return std::unique_ptr<IDataAccess>(new HTTP(uri.toString()));

	return std::unique_ptr<IDataAccess>(new File(aUrl));;
}
