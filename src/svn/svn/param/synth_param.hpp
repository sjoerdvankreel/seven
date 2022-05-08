#ifndef SVN_PARAM_SYNTH_PARAM_HPP
#define SVN_PARAM_SYNTH_PARAM_HPP

#include <svn/param/param_id.hpp>

namespace svn {

struct synth_param
{
  param_id const id;
  char const* const name;
  char const* const description;
  char const* const unit;
  double const default_value;
};

} // namespace svn
#endif // SVN_PARAM_SYNTH_PARAM_HPP