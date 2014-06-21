#ifndef THREE_MACROS_UTILS_H
#define THREE_MACROS_UTILS_H

#include <cassert>

#include <three/config.h>

#define THREE_IMPL_OBJECT(NAME) \
typedef std::shared_ptr<NAME> Ptr; \
Ptr cloneAndCast() const { return std::static_pointer_cast<NAME>(clone()); } \
virtual THREE::Type type() const THREE_OVERRIDE { return THREE::NAME; }  \
virtual void visit( Visitor& v ) THREE_OVERRIDE { v( *this ); }           \
virtual void visit( ConstVisitor& v ) const THREE_OVERRIDE { v( *this ); } \
virtual void visit( ConstRawPointerVisitor& v ) const THREE_OVERRIDE { v( &*this ); }

#define THREE_IMPL_MATERIAL(NAME) \
typedef std::shared_ptr<NAME> Ptr; \
Ptr cloneAndCast() const { return std::static_pointer_cast<NAME>(clone()); } \
virtual ~NAME() {} \
virtual THREE::MaterialType type() const THREE_OVERRIDE { return THREE::NAME; } \
protected: \
virtual void __clone( Material::Ptr& cloned ) const THREE_OVERRIDE { \
  if ( !cloned ) { cloned = create(); } return Material::__clone( cloned ); } \
public:

#define THREE_IMPL_VISITOR(NAME)                                       \
virtual void visit( Visitor& v ) THREE_OVERRIDE { v( *this ); }           \
virtual void visit( ConstVisitor& v ) const THREE_OVERRIDE { v( *this ); } \
virtual void visit( ConstRawPointerVisitor& v ) const THREE_OVERRIDE { v( &*this ); }

#define THREE_TYPE(NAME)                                       \
virtual THREE::Type type() const { return THREE::NAME; }

#define THREE_ASSERT assert

#endif //THREE_MACROS_UTILS_H