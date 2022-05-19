#ifndef SVN_SUPPORT_AUDIO_SAMPLE_HPP
#define SVN_SUPPORT_AUDIO_SAMPLE_HPP

#include <cstdint>

namespace svn {

struct audio_sample
{
  float left;
  float right;

  audio_sample& operator+=(float y);
  audio_sample& operator-=(float y);
  audio_sample& operator*=(float y);
  audio_sample& operator/=(float y);

  audio_sample& operator+=(audio_sample y);
  audio_sample& operator-=(audio_sample y);
  audio_sample& operator*=(audio_sample y);
  audio_sample& operator/=(audio_sample y);
};

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
#endif // SVN_SUPPORT_AUDIO_SAMPLE_HPP