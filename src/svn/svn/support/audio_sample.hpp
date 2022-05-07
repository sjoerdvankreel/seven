#ifndef SVN_SUPPORT_AUDIO_SAMPLE_HPP
#define SVN_SUPPORT_AUDIO_SAMPLE_HPP

namespace svn {

template <typename sample_type>
struct audio_sample
{
  sample_type left;
  sample_type right;

  audio_sample<float> to_float() const;
  audio_sample<double> to_double() const;

  audio_sample& operator+=(sample_type y);
  audio_sample& operator-=(sample_type y);
  audio_sample& operator*=(sample_type y);
  audio_sample& operator/=(sample_type y);

  audio_sample& operator+=(audio_sample y);
  audio_sample& operator-=(audio_sample y);
  audio_sample& operator*=(audio_sample y);
  audio_sample& operator/=(audio_sample y);
};

template <typename sample_type>
audio_sample<sample_type>& 
operator+(
  audio_sample<sample_type> x,
  sample_type y) {
  return { x.left + y, x.right + y; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator-(
  audio_sample<sample_type> x,
  sample_type y) {
  return { x.left - y, x.right - y; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator*(
  audio_sample<sample_type> x,
  sample_type y) {
  return { x.left * y, x.right * y; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator/(
  audio_sample<sample_type> x,
  sample_type y) {
  return { x.left / y, x.right / y; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator+(
  sample_type x,
  audio_sample<sample_type> y) {
  return { x + y.left, x + y.right; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator-(
  sample_type x,
  audio_sample<sample_type> y) {
  return { x - y.left, x - y.right; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator*(
  sample_type x,
  audio_sample<sample_type> y) {
  return { x * y.left, x * y.right; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator/(
  sample_type x,
  audio_sample<sample_type> y) {
  return { x / y.left, x / y.right; } }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator+=(
  sample_type y) {
  left += y; right += y; return *this; }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator-=(
  sample_type y) {
  left -= y; right -= y; return *this; }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator*=(
  sample_type y) {
  left *= y; right *= y; return *this; }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator/=(
  sample_type y) {
  left /= y; right /= y; return *this; }
  
template <typename sample_type>
audio_sample<sample_type>& 
operator+(
  audio_sample<sample_type> x,
  audio_sample<sample_type> y) {
  return { x.left + y.left, x.right + y.right; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator-(
  audio_sample<sample_type> x,
  audio_sample<sample_type> y) {
  return { x.left - y.left, x.right - y.right; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator*(
  audio_sample<sample_type> x,
  audio_sample<sample_type> y) {
  return { x.left * y.left, x.right * y.right; } }

template <typename sample_type>
audio_sample<sample_type>& 
operator/(
  audio_sample<sample_type> x,
  audio_sample<sample_type> y) {
  return { x.left / y.left, x.right / y.right; } }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator+=(
  audio_sample<sample_type> y) {
  left += y.left; right += y.right; return *this; }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator-=(
  audio_sample<sample_type> y) {
  left -= y.left; right -= y.right; return *this; }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator*=(
  audio_sample<sample_type> y) {
  left *= y.left; right *= y.right; return *this; }

template <typename sample_type>
audio_sample<sample_type>&
audio_sample<sample_type>::operator/=(
  audio_sample<sample_type> y) {
  left /= y.left; right /= y.right; return *this; }

template <>
audio_sample<float>
audio_sample<double>::to_float() const {
  return { static_cast<float>(left), static_cast<float>(right) }; }

template <>
audio_sample<double>
audio_sample<float>::to_double() const {
  return { static_cast<double>(left), static_cast<double>(right) }; }

} // namespace svn
#endif // SVN_SUPPORT_AUDIO_SAMPLE_HPP