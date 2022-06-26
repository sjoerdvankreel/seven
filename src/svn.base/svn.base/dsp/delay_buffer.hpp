#ifndef SVN_BASE_DSP_DELAY_BUFFER_HPP
#define SVN_BASE_DSP_DELAY_BUFFER_HPP

#include <cstring>
#include <cassert>

namespace svn::base {

template <class T, size_t N>
class delay_buffer
{
  T _buffer[N];
  size_t _head;
public:
  delay_buffer();
  void clear();
  void push(T val);
  T get(size_t pos) const;
};

template <class T, size_t N>
delay_buffer<T, N>::delay_buffer() : _head(0)
{ clear(); }

template <class T, size_t N>
inline void
delay_buffer<T, N>::clear()
{ std::memset(_buffer, 0, sizeof(_buffer)); }

template <class T, size_t N>
inline void
delay_buffer<T, N>::push(T val)
{
  _buffer[_head++] = val;
  _head %= N;
}

template <class T, size_t N>
inline T 
delay_buffer<T, N>::get(size_t pos) const
{ 
  assert(0 <= pos && pos < N);
  return _buffer[(_head + N - pos - 1) % N]; 
}

} // namespace svn::base
#endif // SVN_BASE_DSP_DELAY_BUFFER_HPP