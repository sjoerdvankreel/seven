#ifndef SVN_DSP_DSP_HPP
#define SVN_DSP_DSP_HPP

#include <svn/support/param_value.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/input_buffer.hpp>

namespace svn {

inline void
clear_audio(
  audio_sample* audio, 
  std::int32_t sample_count)
{
  for(std::int32_t s = 0; s < sample_count; s++)
    audio[s].left = audio[s].right = 0.0f;
}

inline void
add_audio(
  audio_sample* x, 
  audio_sample* y, 
  std::int32_t sample_count)
{
  for (std::int32_t s = 0; s < sample_count; s++)
    x[s] += y[s];
}

inline float
automate_real(
  input_buffer const& input, 
  param_value* state, 
  std::int32_t param, 
  std::int32_t sample)
{
  auto automation = static_cast<float*>(input.automation[param]);
  return state[param].real = automation[sample];
}

inline float
automate_discrete(
  input_buffer const& input, 
  param_value* state, 
  std::int32_t param, 
  std::int32_t sample)
{
  auto automation = static_cast<std::int32_t*>(input.automation[param]);
  return state[param].discrete = automation[sample];
}

} // namespace svn
#endif // SVN_DSP_DSP_HPP