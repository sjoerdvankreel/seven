#ifndef SVN_PARAM_PARAM_INFO_HPP
#define SVN_PARAM_PARAM_INFO_HPP

namespace svn {

struct param_info
{
  char const* const short_name;
  char const* const unit;
  double const default_value;
};

} // namespace svn
#endif // SVN_PARAM_PARAM_INFO_HPP