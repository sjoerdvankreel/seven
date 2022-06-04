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

inline float
sanity(float val)
{
  assert(!std::isnan(val));
  assert(!std::isinf(val));
  assert(std::fpclassify(val) != FP_SUBNORMAL);
  return val;
}

inline float 
sanity_bipolar(float val)
{
  sanity(val);
  assert(val <= 1.0f);
  assert(val >= -1.0f);
  return val;
}

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

} // namespace svn::base
#endif // SVN_BASE_DSP_DSP_HPP