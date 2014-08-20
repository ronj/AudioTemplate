#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

#include <common/config.h>

class NonCopyable
{
public:
  NonCopyable() CFG_DECL_DEFAULT;

private:
  NonCopyable& operator=(const NonCopyable&) CFG_DECL_DELETE;
  NonCopyable(const NonCopyable&) CFG_DECL_DELETE;
};

#endif // NONCOPYABLE_H
