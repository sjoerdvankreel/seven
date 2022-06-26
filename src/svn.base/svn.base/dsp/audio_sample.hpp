#ifndef SVN_BASE_DSP_AUDIO_SAMPLE_HPP
#define SVN_BASE_DSP_AUDIO_SAMPLE_HPP

namespace svn::base {

template <class T>
struct audio_sample
{
  T left;
  T right;

  T mono() const;
  audio_sample& operator=(T y);
  audio_sample<float> to32() const;
  audio_sample<double> to64() const;

  audio_sample& operator+=(T y);
  audio_sample& operator-=(T y);
  audio_sample& operator*=(T y);
  audio_sample& operator/=(T y);

  audio_sample& operator+=(audio_sample y);
  audio_sample& operator-=(audio_sample y);
  audio_sample& operator*=(audio_sample y);
  audio_sample& operator/=(audio_sample y);
};

typedef audio_sample<float> audio_sample32;
typedef audio_sample<double> audio_sample64;

template <class T> inline T
audio_sample<T>::mono() const
{ return left + right; }

template <class T> inline audio_sample<T>&
audio_sample<T>::operator=(T y)
{ left = right = y; return *this; }

template <> inline audio_sample<float>
audio_sample<double>::to32() const
{ return { static_cast<float>(left), static_cast<float>(right) }; }

template <> inline audio_sample<double>
audio_sample<float>::to64() const
{ return { static_cast<double>(left), static_cast<double>(right) }; }

template <class T> inline audio_sample<T>
operator+(audio_sample<T> x, T y)
{ return { x.left + y, x.right + y }; }
template <class T> inline audio_sample<T>
operator-(audio_sample<T> x, T y)
{ return { x.left - y, x.right - y }; }
template <class T>inline audio_sample<T>
operator*(audio_sample<T> x, T y)
{ return { x.left * y, x.right * y }; }
template <class T> inline audio_sample<T>
operator/(audio_sample<T> x, T y)
{ return { x.left / y, x.right / y }; }

template <class T> inline audio_sample<T>
operator+(T x, audio_sample<T> y)
{ return { x + y.left, x + y.right }; }
template <class T> inline audio_sample<T>
operator-(T x, audio_sample<T> y)
{ return { x - y.left, x - y.right }; }
template <class T> inline audio_sample<T>
operator*(T x, audio_sample<T> y)
{ return { x * y.left, x * y.right }; }
template <class T> inline audio_sample<T>
operator/(T x, audio_sample<T> y)
{ return { x / y.left, x / y.right }; }

template <class T> inline audio_sample<T>&
audio_sample<T>::operator+=(T y)
{ left += y; right += y; return *this; }
template <class T> inline audio_sample<T>&
audio_sample<T>::operator-=(T y)
{ left -= y; right -= y; return *this; }
template <class T> inline audio_sample<T>&
audio_sample<T>::operator*=(T y)
{ left *= y; right *= y; return *this; }
template <class T> inline audio_sample<T>&
audio_sample<T>::operator/=(T y)
{ left /= y; right /= y; return *this; }

template <class T> inline audio_sample<T>
operator+(audio_sample<T> x, audio_sample<T> y)
{ return { x.left + y.left, x.right + y.right }; }
template <class T> inline audio_sample<T>
operator-(audio_sample<T> x, audio_sample<T> y)
{ return { x.left - y.left, x.right - y.right }; }
template <class T> inline audio_sample<T>
operator*(audio_sample<T> x, audio_sample<T> y)
{ return { x.left * y.left, x.right * y.right }; }
template <class T> inline audio_sample<T>
operator/(audio_sample<T> x, audio_sample<T> y)
{ return { x.left / y.left, x.right / y.right }; }

template <class T> inline audio_sample<T>&
audio_sample<T>::operator+=(audio_sample<T> y)
{ left += y.left; right += y.right; return *this; }
template <class T> inline audio_sample<T>&
audio_sample<T>::operator-=(audio_sample<T> y)
{ left -= y.left; right -= y.right; return *this; }
template <class T> inline audio_sample<T>&
audio_sample<T>::operator*=(audio_sample<T> y)
{ left *= y.left; right *= y.right; return *this; }
template <class T> inline audio_sample<T>&
audio_sample<T>::operator/=(audio_sample<T> y)
{ left /= y.left; right /= y.right; return *this; }  

} // namespace svn::base
#endif // SVN_BASE_DSP_AUDIO_SAMPLE_HPP