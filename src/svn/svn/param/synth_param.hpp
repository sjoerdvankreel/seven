#ifndef SVN_PARAM_SYNTH_PARAM_HPP
#define SVN_PARAM_SYNTH_PARAM_HPP

#include <svn/param/param_id.hpp>
#include <svn/param/param_info.hpp>

namespace svn {

struct synth_param
{
  param_id const id;
  char const* const long_name;
  param_info const info;
};

} // namespace svn
#endif // SVN_PARAM_SYNTH_PARAM_HPP