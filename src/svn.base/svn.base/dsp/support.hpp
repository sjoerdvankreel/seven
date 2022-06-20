#ifndef SVN_BASE_DSP_SUPPORT_HPP
#define SVN_BASE_DSP_SUPPORT_HPP

#include <svn.base/dsp/io_types.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <cmath>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::base {

inline std::int32_t constexpr midi_note_c4 = 60;

// Note including cents.
inline float
note_to_frequency(float note)
{ return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f); }

inline void
clear_audio(audio_sample * audio, std::int32_t sample_count)
{
  assert(audio != nullptr);
  assert(sample_count >= 0);
  for (std::int32_t s = 0; s < sample_count; s++)
    audio[s].left = audio[s].right = 0.0f;
}

inline bool
clip_audio(audio_sample* audio, std::int32_t sample_count)
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
add_audio(audio_sample* x, audio_sample* y, std::int32_t sample_count)
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
  return param_value(descriptor.discrete.to_range(val.discrete));
}

} // namespace svn::base
#endif // SVN_BASE_DSP_SUPPORT_HPP