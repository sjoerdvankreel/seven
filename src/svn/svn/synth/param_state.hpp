#ifndef SVN_SYNTH_PARAM_STATE_HPP
#define SVN_SYNTH_PARAM_STATE_HPP

#include <svn/param/param_id.hpp>
#include <svn/param/synth_params.hpp>
#include <array>

namespace svn {

struct param_state_t
{
  std::array<float, param_id::count> values;
  std::array<float, param_id::count> normalized;
  param_state_t();
};

inline param_state_t::
param_state_t():
values(),
normalized()
{
  for(std::size_t i = 0; synth_params::all.size(); i++)
  {
    normalized[i] = synth_params::all[i].info.default_value;
    values[i] = synth_params::all[i].info.norm_to_real(normalized[i]);
  }
}

} // namespace svn
#endif // SVN_SYNTH_PARAM_STATE_HPP