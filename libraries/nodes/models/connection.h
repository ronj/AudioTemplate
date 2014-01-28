#ifndef CONNECTION_H
#define CONNECTION_H

#include <nodes/utils/indexer.h>

class Field;

class Connection
{
public:
  Connection(Field& aInField, Field& aOutField);
  ~Connection();

  Field& toField() const;
  Field& fromField() const;

private:
  bool isValid() const;
  void switchFieldsIfNeeded();

private:
  Indexer iIndexer;
  unsigned int iConnectionID;
  Field* iFromField;
  Field* iToField;
};

#endif // CONNECTION_H
