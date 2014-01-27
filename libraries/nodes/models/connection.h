#ifndef CONNECTION_H
#define CONNECTION_H

#include <algorithm>
#include <memory>
#include <cassert>

#include <nodes/utils/indexer.h>
#include <nodes/models/field.h>

class Connection
{
public:
  Connection(Field& aInField, Field& aOutField)
    : iConnectionID(iIndexer.getUID())
    , iFromField(&aInField)
    , iToField(&aOutField)
  {
    if (isValid())
    {
      switchFieldsIfNeeded();

      // An input field can only have one connection.
      iToField->removeConnections();

      iFromField->addConnection(this);
      iToField->addConnection(this);

      iToField->setValue(iFromField->getValue());
      iFromField->getNode().setDirty();
    }
  }

  ~Connection()
  {
    iFromField->unregisterConnection(this);
    iToField->unregisterConnection(this);
    iToField->removeConnections();

    iToField->getNode().setDirty();
    iToField->setChanged();
  }

  Field& toField()
  {
    assert(iToField != nullptr);
    return *iToField;
  }

  Field fromField()
  {
    assert(iFromField != nullptr);
    return *iFromField;
  }

private:
  bool isValid()
  {
    if ((iFromField == nullptr) || (iToField == nullptr))
    {
      return false;
    }

    // Never connect two inputs or two outputs together.
    if (iFromField->isOutput() == iToField->isOutput())
    {
      return false;
    }

    // Never connect an input and output of the same node together.
    if (iFromField->getNode().nodeID() == iToField->getNode().nodeID())
    {
      return false;
    }

    return true;
  }

  void switchFieldsIfNeeded()
  {
    if (iFromField->isOutput() == false)
    {
      std::swap(iFromField, iToField);
    }
  }

private:
  Indexer iIndexer;
  unsigned int iConnectionID;
  Field* iFromField;
  Field* iToField;
};

#endif // CONNECTION_H
