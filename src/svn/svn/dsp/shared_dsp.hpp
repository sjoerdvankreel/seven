#ifndef SVN_DSP_SHARED_DSP_HPP
#define SVN_DSP_SHARED_DSP_HPP

#include <svn/support/topo_rt.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/param_value.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/input_buffer.hpp>

#include <cassert>

namespace svn {

inline void
clear_audio(
  audio_sample* audio, 
  std::int32_t sample_count)
{
  assert(audio != nullptr);
  assert(sample_count >= 0);
  for(std::int32_t s = 0; s < sample_count; s++)
    audio[s].left = audio[s].right = 0.0f;
}

inline void
add_audio(
  audio_sample* x, 
  audio_sample* y, 
  std::int32_t sample_count)
{
  assert(x != nullptr);
  assert(y != nullptr);
  assert(sample_count >= 0);
  for (std::int32_t s = 0; s < sample_count; s++)
    x[s] += y[s];
}

inline float
param_real_to_actual(std::int32_t param, float val)
{
  param_value result;
  param_info const& info = *synth_params[param].info;
  float min = info.real.min;
  float range = info.real.max - min;
  assert(info.type == param_type::real);

  switch (info.real.slope)
  {
  case param_slope::linear: return min + range * val;
  case param_slope::quadratic: return min + range * val * val;
  default: assert(false); return 0.0f;
  }
}

inline float
automate_discrete(
  input_buffer const& input,
  param_value* state,
  std::int32_t param,
  std::int32_t sample)
{
  assert(param >= 0);
  assert(sample >= 0);
  assert(state != nullptr);
  auto automation = static_cast<std::int32_t*>(input.automation[param]);
  return state[param].discrete = automation[sample];
}

inline float
automate_real(
  input_buffer const& input, 
  param_value* state, 
  std::int32_t param, 
  std::int32_t sample)
{
  assert(param >= 0);
  assert(sample >= 0);
  assert(state != nullptr);
  auto automation = static_cast<float*>(input.automation[param]);
  return param_real_to_actual(param, static_cast<float>(state[param].real = automation[sample]));
}

} // namespace svn
#endif // SVN_DSP_SHARED_DSP_HPP