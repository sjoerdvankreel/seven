#ifndef SVN_BASE_DSP_SUPPORT_HPP
#define SVN_BASE_DSP_SUPPORT_HPP

#include <svn.base/dsp/io_types.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <cmath>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::base {

inline float constexpr sanity_epsilon = 1.0e-5f;
inline std::int32_t constexpr midi_note_c4 = 60;

// Note including cents.
inline float
note_to_frequency(float note)
{ return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f); }

// E.g. "3/4" -> sample count.
inline float
timesig_to_samples(float sample_rate, float bpm, float timesig_val)
{ return sample_rate * 60.0f / bpm * timesig_val; }

template <class T> inline T
sanity(T val)
{
  assert(!std::isnan(val));
  assert(!std::isinf(val));
  assert(std::fpclassify(val) != FP_SUBNORMAL);
  return val;
}

template <class T> inline T
sanity_unipolar(T val)
{
  sanity(val);
  assert(val <= 1.0f + sanity_epsilon);
  assert(val >= 0.0f - sanity_epsilon);
  return val;
}

template <class T> inline T
sanity_bipolar(T val)
{
  sanity(val);
  assert(val <= 1.0f + sanity_epsilon);
  assert(val >= -1.0f - sanity_epsilon);
  return val;
}

template <class T> inline audio_sample<T>
sanity_audio(audio_sample<T> val)
{
  sanity(val.left);
  sanity(val.right);
  return val;
}

template <class T> inline audio_sample<T>
sanity_audio_bipolar(audio_sample<T> val)
{
  sanity_bipolar(val.left);
  sanity_bipolar(val.right);
  return val;
}

inline std::uint64_t
next_pow2(std::uint64_t x)
{
  uint64_t result = 0;
  if (x == 0) return 0;
  if (x && !(x & (x - 1))) return x;
  while (x != 0) x >>= 1, result++;
  return 1ULL << result;
}

inline bool
clip_audio(audio_sample32* audio, std::int32_t sample_count)
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

inline void
add_audio(audio_sample32* x, audio_sample32 const* y, std::int32_t sample_count)
{
  assert(x != nullptr);
  assert(y != nullptr);
  assert(sample_count >= 0);
  for (std::int32_t s = 0; s < sample_count; s++)
    x[s] += y[s];
}

inline param_value
transform_to_dsp(topology_info const* topology, std::int32_t param, param_value val)
{
  auto const& descriptor = *topology->params[param].descriptor;
  if(descriptor.type == param_type::real)
    return param_value(descriptor.real.dsp.to_range(val.real));
  return param_value(val.discrete);
}

} // namespace svn::base
#endif // SVN_BASE_DSP_SUPPORT_HPP