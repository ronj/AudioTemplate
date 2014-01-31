#include "connection.h"

#include <nodes/models/field.h>
#include <nodes/models/node.h>

#include <algorithm>
#include <cassert>

Connection::Connection(Field& aInField, Field& aOutField)
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
    iFromField->getNode().setDirty(true);
  }
}

Connection::~Connection()
{
  iFromField->unregisterConnection(this);
  iToField->unregisterConnection(this);
  iToField->removeConnections();

  iToField->getNode().setDirty(true);
  iToField->setChanged(true);
}

Field& Connection::toField() const
{
  assert(iToField != nullptr);
  return *iToField;
}

Field& Connection::fromField() const
{
  assert(iFromField != nullptr);
  return *iFromField;
}

bool Connection::isValid() const
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

void Connection::switchFieldsIfNeeded()
{
  if (iFromField->isOutput() == false)
  {
    std::swap(iFromField, iToField);
  }
}
