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

// Any cv input e.g. lfos, envelopes.
struct cv_sample
{
  float value = 0.0f;
  bool bipolar = false;
};

// Simplify access to static state.
struct state_view
{
  topology_info const* const topology;
  param_value const* const state;
  std::int32_t const part_type;
  std::int32_t const part_index;

  float get_real_dsp(std::int32_t param) const;
  std::int32_t get_discrete(std::int32_t param) const;
};

inline std::int32_t
state_view::get_discrete(std::int32_t param) const
{
  std::int32_t index = topology->param_bounds[part_type][part_index] + param;
  assert(topology->params[index].descriptor->type != param_type::real);
  return state[index].discrete;
}

inline float
state_view::get_real_dsp(std::int32_t param) const
{
  std::int32_t index = topology->param_bounds[part_type][part_index] + param;
  assert(topology->params[index].descriptor->type == param_type::real);
  return topology->params[index].descriptor->real.dsp.to_range(state[index].real);
}

// Midi -> frequency lookup.
struct note_to_frequency_table_init
{
  static inline constexpr std::int32_t note_count = 12;
  static inline constexpr std::int32_t cent_count = 100;
  static inline constexpr std::int32_t octave_count = 12;
  static inline constexpr std::int32_t frequency_count = octave_count * note_count * cent_count;

  float table[frequency_count];
  note_to_frequency_table_init();
  static note_to_frequency_table_init const init;
};

// Note including cents.
inline float
note_to_frequency(float midi)
{ return 440.0f * std::pow(2.0f, (midi - 69.0f) / 12.0f); }
inline float
note_to_frequency_table(std::int32_t midi, float cent)
{
  float target = (midi * note_to_frequency_table_init::cent_count) + cent;
  std::int32_t cent_low = std::max(0, static_cast<std::int32_t>(target));
  std::int32_t cent_high = std::min(note_to_frequency_table_init::frequency_count - 1, cent_low + 1);
  float mix = target - cent_low;
  float freq_low = note_to_frequency_table_init::init.table[cent_low];
  float freq_high = note_to_frequency_table_init::init.table[cent_high];
  return (1.0f - mix) * freq_low + mix * freq_high;
}

// E.g. "3/4" -> sample count.
inline float
timesig_to_samples(float sample_rate, float bpm, float timesig_val)
{ return sample_rate * 60.0f / bpm * timesig_val; }
inline float
timesig_to_frequency(float sample_rate, float bpm, float timesig_val)
{ return bpm / (60.0f * timesig_val) ; }

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

} // namespace svn::base
#endif // SVN_BASE_DSP_SUPPORT_HPP