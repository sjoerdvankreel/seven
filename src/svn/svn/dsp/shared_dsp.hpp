#ifndef SVN_DSP_SHARED_DSP_HPP
#define SVN_DSP_SHARED_DSP_HPP

#include <svn/support/topo_rt.hpp>
#include <svn/support/param_value.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/input_buffer.hpp>
#include <svn/support/param_transform.hpp>

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
  auto const& real_info = synth_params[param].info->real;
  auto automation = static_cast<float*>(input.automation[param]);
  float result = static_cast<float>(state[param].real = automation[sample]);
  return param_real_to_range(real_info.slope, result, real_info.min, real_info.max);
}

} // namespace svn
#endif // SVN_DSP_SHARED_DSP_HPP