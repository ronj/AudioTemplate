#ifndef THREE_MACROS_UTILS_H
#define THREE_MACROS_UTILS_H

#include <cassert>

#define THREE_IMPL_OBJECT(NAME)                                       \
virtual enums::Type type() const { return enums::NAME; }  \
virtual void visit( Visitor& v ) { v( *this ); }           \
virtual void visit( ConstVisitor& v ) const { v( *this ); } \
virtual void visit( ConstRawPointerVisitor& v ) const { v( &*this ); }

#define THREE_IMPL_VISITOR(NAME)                                       \
virtual void visit( Visitor& v ) { v( *this ); }           \
virtual void visit( ConstVisitor& v ) const { v( *this ); } \
virtual void visit( ConstRawPointerVisitor& v ) const { v( &*this ); }

#define THREE_TYPE(NAME)                                       \
virtual enums::Type type() const { return enums::NAME; }

#define THREE_ASSERT assert

#endif //THREE_MACROS_UTILS_H