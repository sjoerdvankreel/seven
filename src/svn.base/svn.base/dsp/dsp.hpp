#ifndef SVN_BASE_DSP_DSP_HPP
#define SVN_BASE_DSP_DSP_HPP

#include <svn.base/dsp/block_input.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <cmath>
#include <cassert>
#include <cstdint>
#include <algorithm>

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

// Note including cents.
inline float
note_to_frequency(float note)
{ return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f); }

inline bool
clip_audio(
  audio_sample* audio,
  std::int32_t sample_count)
{
  bool result = false;
  for (std::int32_t s = 0; s < sample_count; s++)
  {
    result |= audio[s].left < -1.0f;
    result |= audio[s].left > 1.0f;
    result |= audio[s].right < -1.0f;
    result |= audio[s].right > 1.0f;
    audio[s].left = std::clamp(audio[s].left, -1.0f, 1.0f);
    audio[s].right = std::clamp(audio[s].right, -1.0f, 1.0f);
  }
  return result;
}

} // namespace svn::base
#endif // SVN_BASE_DSP_DSP_HPP