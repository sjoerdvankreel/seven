#ifndef SVN_PARAM_PARAM_INFO_HPP
#define SVN_PARAM_PARAM_INFO_HPP

namespace svn {

typedef float (*norm_to_real_t)(float norm);

struct param_info
{
  wchar_t const* const short_name;
  wchar_t const* const unit;
  float const default_value;
  norm_to_real_t norm_to_real;
  static inline norm_to_real_t const identity = [](float x) { return x; };
};

} // namespace svn
#endif // SVN_PARAM_PARAM_INFO_HPP