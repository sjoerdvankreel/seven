#ifndef SVN_SYNTH_DSP_SUPPORT_HPP
#define SVN_SYNTH_DSP_SUPPORT_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.base/support/support.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/dsp/automation_view.hpp>
#include <svn.synth/topology/topology.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::synth {

// Voice block input.
struct voice_input
{
  float bpm;
  std::int32_t sample_count;
  std::int64_t stream_position;
  base::automation_view automation;
};

// Internal cv state.
class cv_state
{
  static inline std::vector<std::vector<std::int32_t>> const input_table_in
  = svn::base::multi_list_table_init_in(cv_input_counts, cv_route_input::count);
  static inline std::vector<std::pair<std::int32_t, std::int32_t>> const input_table_out
  = svn::base::multi_list_table_init_out(cv_input_counts, cv_route_input::count);
  static inline std::vector<std::vector<std::vector<std::int32_t>>> const output_table_in
  = svn::base::zip_list_table_init_in(cv_output_counts, cv_output_target_counts, cv_route_output::count);

  // Of size max parameter count per part type * max sample count.
  std::vector<float*> scratch;
  std::vector<float> scratch_buffer;
  float const* input_buffer(std::int32_t input, std::int32_t index) const;

public:
  explicit cv_state(std::int32_t max_sample_count);
  std::array<std::vector<float>, envelope_count> envelope;
  std::array<std::vector<float>, voice_lfo_count> voice_lfo;

  float const* const* mix(voice_input const& input,
    cv_route_output route_output, std::int32_t route_index);
};

inline cv_state::
cv_state(std::int32_t max_sample_count): 
envelope(), voice_lfo(), scratch(), scratch_buffer()
{ 
  std::vector<float> cv(static_cast<std::size_t>(max_sample_count));
  envelope.fill(cv);
  voice_lfo.fill(cv);
  std::int32_t max_outputs = *std::max_element(cv_output_target_counts, cv_output_target_counts + cv_route_output::count);
  scratch_buffer.resize(static_cast<std::size_t>(max_outputs * max_sample_count));
  for(std::int32_t p = 0; p < max_outputs; p++)
    scratch.push_back(scratch_buffer.data() + p * max_sample_count);
}

inline float const*
cv_state::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case cv_route_input::off: return nullptr;
  case cv_route_input::vlfo: return voice_lfo[index].data();
  case cv_route_input::envelope: return envelope[index].data();
  default: assert(false); return nullptr;
  }
}

inline float const* const*
cv_state::mix(voice_input const& input,
  cv_route_output route_output, std::int32_t route_index)
{
  std::int32_t input_off = input_table_in[cv_route_input::off][0];
  base::automation_view automation = input.automation.rearrange_params(part_type::cv_route, 0);
  for(std::int32_t p = 0; p < cv_output_target_counts[route_output]; p++)
  {
    std::int32_t output_id = output_table_in[route_output][route_index][p];
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      scratch[p][s] = 0.0f;
      for (std::int32_t i = 0; i < cv_route_count; i++)
      {
        // +2 = plot parameters
        std::int32_t in = automation.get(i * 3 + 2, s).discrete;
        if (in == input_off) continue;
        std::int32_t out = automation.get(i * 3 + 2 + 1, s).discrete;
        if (out != output_id) continue;
        float amt = automation.get(i * 3 + 2 + 2, s).real;
        std::pair<std::int32_t, std::int32_t> input_ids(input_table_out[in]);
        scratch[p][s] = base::sanity_bipolar(scratch[p][s] + input_buffer(input_ids.first, input_ids.second)[s] * amt);
      }
    }
  }
  return scratch.data();
}

// Internal audio outputs.
class audio_state
{
  static inline std::vector<std::vector<std::int32_t>> const input_table_in
  = svn::base::multi_list_table_init_in(audio_input_counts, audio_route_input::count);
  static inline std::vector<std::vector<std::int32_t>> const output_table_in
  = svn::base::multi_list_table_init_in(audio_output_counts, audio_route_output::count);
  static inline std::vector<std::pair<std::int32_t, std::int32_t>> const input_table_out
  = svn::base::multi_list_table_init_out(audio_input_counts, audio_route_input::count);

  std::vector<base::audio_sample32> scratch;
  base::audio_sample32 const* input_buffer(std::int32_t input, std::int32_t index) const;

public:
  std::vector<base::audio_sample32> voice_amp;
  std::array<std::vector<base::audio_sample32>, oscillator_count> oscillator;
  std::array<std::vector<base::audio_sample32>, voice_filter_count> voice_filter;
  
  explicit audio_state(std::int32_t max_sample_count);
  svn::base::audio_sample32 const* mix(voice_input const& input, 
    audio_route_output route_output, std::int32_t route_index);
};

inline audio_state::
audio_state(std::int32_t max_sample_count): 
oscillator(), voice_filter()
{
  std::vector<base::audio_sample32> audio(static_cast<std::size_t>(max_sample_count));
  scratch = audio;
  voice_amp = audio;
  oscillator.fill(audio);
  voice_filter.fill(audio);
}

inline base::audio_sample32 const*
audio_state::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case audio_route_input::off: return nullptr;
  case audio_route_input::osc: return oscillator[index].data();
  case audio_route_input::filter: return voice_filter[index].data();
  default: assert(false); return nullptr;
  }
}

inline svn::base::audio_sample32 const*
audio_state::mix(voice_input const& input,
  audio_route_output route_output, std::int32_t route_index)
{
  std::int32_t input_off = input_table_in[audio_route_input::off][0];
  std::int32_t output_id = output_table_in[route_output][route_index];
  base::automation_view automation = input.automation.rearrange_params(part_type::audio_route, 0);
  for(std::int32_t s = 0; s < input.sample_count; s++)
  {
    scratch[s] = { 0.0f, 0.0f };
    for (std::int32_t i = 0; i < audio_route_count; i++)
    {
      std::int32_t in = automation.get(i * 3, s).discrete;
      if (in == input_off) continue;
      std::int32_t out = automation.get(i * 3 + 1, s).discrete;
      if (out != output_id) continue;
      float amt = automation.get(i * 3 + 2, s).real;
      std::pair<std::int32_t, std::int32_t> input_ids(input_table_out[in]);
      scratch[s] = base::sanity_audio(scratch[s] + input_buffer(input_ids.first, input_ids.second)[s] * amt);
    }
  }
  return scratch.data();
}

} // namespace svn::base
#endif // SVN_SYNTH_DSP_SUPPORT_HPP