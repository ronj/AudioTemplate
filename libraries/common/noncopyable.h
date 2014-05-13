#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable
{
public:
  noncopyable() = default;

private:
  noncopyable& operator=(const noncopyable&) = delete;
  noncopyable(const noncopyable&) = delete;
};

#endif // NONCOPYABLE_H
