#ifndef DATA_ACCESS_FACTORY_H
#define DATA_ACCESS_FACTORY_H

#include "data_access.h"

#include <memory>

class DataAccessFactory
{
public:
	static std::unique_ptr<IDataAccess> CreateForUrl(const std::string& aUrl);
};

#endif // !DATA_ACCESS_FACTORY_H
