#ifndef SVN_SUPPORT_AUDIO_HPP
#define SVN_SUPPORT_AUDIO_HPP

#include <cstdint>

namespace svn {

struct audio_sample
{
  float left;
  float right;

  void clear();

  audio_sample& operator+=(float y);
  audio_sample& operator-=(float y);
  audio_sample& operator*=(float y);
  audio_sample& operator/=(float y);

  audio_sample& operator+=(audio_sample y);
  audio_sample& operator-=(audio_sample y);
  audio_sample& operator*=(audio_sample y);
  audio_sample& operator/=(audio_sample y);
};

struct audio_buffer
{
  audio_sample* samples;
  void clear(std::int32_t count);
  void add(audio_buffer y, std::int32_t count);
}; 

inline void
audio_buffer::clear(std::int32_t count)
{
  for(std::int32_t i = 0; i < count; i++)
    samples[i].clear();
}

inline void
audio_buffer::add(audio_buffer y, std::int32_t count)
{
  for(std::int32_t i = 0; i < count; i++)
    samples[i] += y.samples[i];
}

inline void
audio_sample::clear() 
{ left = right = 0.0f; }

inline audio_sample
operator+(audio_sample x, float y)
{ return { x.left + y, x.right + y }; }
inline audio_sample
operator-(audio_sample x, float y)
{ return { x.left - y, x.right - y }; }
inline audio_sample
operator*(audio_sample x, float y)
{ return { x.left * y, x.right * y }; }
inline audio_sample
operator/(audio_sample x, float y)
{ return { x.left / y, x.right / y }; }

inline audio_sample
operator+(float x, audio_sample y)
{ return { x + y.left, x + y.right }; }
inline audio_sample
operator-(float x, audio_sample y)
{ return { x - y.left, x - y.right }; }
inline audio_sample
operator*(float x, audio_sample y)
{ return { x * y.left, x * y.right }; }
inline audio_sample
operator/(float x, audio_sample y)
{ return { x / y.left, x / y.right }; }

inline audio_sample& 
audio_sample::operator+=(float y)
{ left += y; right += y; return *this; }
inline audio_sample& 
audio_sample::operator-=(float y)
{ left -= y; right -= y; return *this; }
inline audio_sample& 
audio_sample::operator*=(float y)
{ left *= y; right *= y; return *this; }
inline audio_sample& 
audio_sample::operator/=(float y)
{ left /= y; right /= y; return *this; }

inline audio_sample
operator+(audio_sample x, audio_sample y) 
{ return { x.left + y.left, x.right + y.right }; }
inline audio_sample
operator-(audio_sample x, audio_sample y) 
{ return { x.left - y.left, x.right - y.right }; }
inline audio_sample
operator*(audio_sample x, audio_sample y) 
{ return { x.left * y.left, x.right * y.right }; }
inline audio_sample
operator/(audio_sample x, audio_sample y) 
{ return { x.left / y.left, x.right / y.right }; }

inline audio_sample& 
audio_sample::operator+=(audio_sample y) 
{ left += y.left; right += y.right; return *this; }
inline audio_sample& 
audio_sample::operator-=(audio_sample y) 
{ left -= y.left; right -= y.right; return *this; }
inline audio_sample& 
audio_sample::operator*=(audio_sample y) 
{ left *= y.left; right *= y.right; return *this; }
inline audio_sample& 
audio_sample::operator/=(audio_sample y) 
{ left /= y.left; right /= y.right; return *this; }  

} // namespace svn
#endif // SVN_SUPPORT_AUDIO_HPP