#ifndef SVN_SYNTH_PARAM_STATE_HPP
#define SVN_SYNTH_PARAM_STATE_HPP

#include <svn/param/param_id.hpp>
#include <svn/param/synth_params.hpp>
#include <array>

namespace svn {

struct param_state_t
{
  std::array<float, param_id::count> values;
  param_state_t();
};

inline param_state_t::
param_state_t():
values()
{
  for(std::size_t i = 0; i < synth_params::all.size(); i++)
    values[i] = synth_params::all[i].info.default_value;
}

} // namespace svn
#endif // SVN_SYNTH_PARAM_STATE_HPP