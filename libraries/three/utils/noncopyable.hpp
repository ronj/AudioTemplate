#ifndef THREE_NONCOPYABLE_HPP
#define THREE_NONCOPYABLE_HPP

namespace three {

class NonCopyable {
protected:
  NonCopyable() {}
  ~NonCopyable() {}
private:
  NonCopyable( const NonCopyable& ) CFG_DECL_DELETE;
  const NonCopyable& operator=( const NonCopyable& ) CFG_DECL_DELETE;
};

} // namespace three

#endif // THREE_NONCOPYABLE_HPP
