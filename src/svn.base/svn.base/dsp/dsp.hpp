#ifndef SVN_BASE_DSP_DSP_HPP
#define SVN_BASE_DSP_DSP_HPP

#include <svn.base/dsp/block_input.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/support/param_value.hpp>
#include <svn.base/runtime/runtime_param.hpp>
#include <svn.base/static/param_descriptor.hpp>

#include <cassert>
#include <cstdint>

namespace svn::base {

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
  block_input const& input, // With automation offset for the current runtime part.
  param_value* state, // With offset for the current runtime part.
  std::int32_t param, // Static param id, e.g. filter_param::resonance.
  std::int32_t sample) // Sample index in the current process block.
{
  assert(param >= 0);
  assert(sample >= 0);
  assert(state != nullptr);
  auto automation = static_cast<std::int32_t*>(input.automation[param]);
  return state[param].discrete = automation[sample];
}

inline float
automate_real(
  block_input const& input, // With automation offset for the current runtime part.
  param_descriptor const* descriptor, // Pointer to static parameter descriptor array, e.g. start of filter parameters.
  param_value* state, // With offset for the current runtime part.
  std::int32_t param, // Static param id, e.g. filter_param::resonance.
  std::int32_t sample) // Sample index in the current process block.
{
  assert(param >= 0);
  assert(sample >= 0);
  assert(state != nullptr);
  auto const& bounds = descriptor[param].dsp;
  auto automation = static_cast<float*>(input.automation[param]);
  return bounds.to_range(static_cast<float>(state[param].real = automation[sample]));
}

} // namespace svn::base
#endif // SVN_BASE_DSP_DSP_HPP