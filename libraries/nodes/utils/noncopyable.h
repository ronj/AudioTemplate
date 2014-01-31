#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class NonCopyable
{
protected:
  NonCopyable() {}
  ~NonCopyable() {}

private:
  NonCopyable(const NonCopyable& other) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif // NONCOPYABLE_H
