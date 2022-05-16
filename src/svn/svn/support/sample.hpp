#ifndef SVN_SUPPORT_SAMPLE_HPP
#define SVN_SUPPORT_SAMPLE_HPP

#include <cstdint>

namespace svn {

struct sample
{
  float left;
  float right;

  sample& operator+=(float y);
  sample& operator-=(float y);
  sample& operator*=(float y);
  sample& operator/=(float y);

  sample& operator+=(sample y);
  sample& operator-=(sample y);
  sample& operator*=(sample y);
  sample& operator/=(sample y);
};

inline sample
operator+(sample x, float y)
{ return { x.left + y, x.right + y }; }
inline sample
operator-(sample x, float y)
{ return { x.left - y, x.right - y }; }
inline sample
operator*(sample x, float y)
{ return { x.left * y, x.right * y }; }
inline sample
operator/(sample x, float y)
{ return { x.left / y, x.right / y }; }

inline sample
operator+(float x, sample y)
{ return { x + y.left, x + y.right }; }
inline sample
operator-(float x, sample y)
{ return { x - y.left, x - y.right }; }
inline sample
operator*(float x, sample y)
{ return { x * y.left, x * y.right }; }
inline sample
operator/(float x, sample y)
{ return { x / y.left, x / y.right }; }

inline sample&
sample::operator+=(float y)
{ left += y; right += y; return *this; }
inline sample&
sample::operator-=(float y)
{ left -= y; right -= y; return *this; }
inline sample&
sample::operator*=(float y)
{ left *= y; right *= y; return *this; }
inline sample&
sample::operator/=(float y)
{ left /= y; right /= y; return *this; }

inline sample
operator+(sample x, sample y)
{ return { x.left + y.left, x.right + y.right }; }
inline sample
operator-(sample x, sample y)
{ return { x.left - y.left, x.right - y.right }; }
inline sample
operator*(sample x, sample y)
{ return { x.left * y.left, x.right * y.right }; }
inline sample
operator/(sample x, sample y)
{ return { x.left / y.left, x.right / y.right }; }

inline sample&
sample::operator+=(sample y)
{ left += y.left; right += y.right; return *this; }
inline sample&
sample::operator-=(sample y)
{ left -= y.left; right -= y.right; return *this; }
inline sample&
sample::operator*=(sample y)
{ left *= y.left; right *= y.right; return *this; }
inline sample&
sample::operator/=(sample y)
{ left /= y.left; right /= y.right; return *this; }  

} // namespace svn
#endif // SVN_SUPPORT_SAMPLE_HPP