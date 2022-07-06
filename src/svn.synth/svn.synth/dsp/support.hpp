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
public:
  explicit cv_state(std::int32_t max_sample_count);
  std::array<std::vector<float>, envelope_count> envelope;
};

inline cv_state::
cv_state(std::int32_t max_sample_count): envelope()
{ envelope.fill(std::vector<float>(static_cast<std::size_t>(max_sample_count))); }

// Internal audio outputs.
class audio_state
{
  static inline std::vector<std::vector<std::int32_t>> const input_table_in
  = svn::base::multi_list_table_init_in(audio_input_counts, audio_inputs_count);
  static inline std::vector<std::vector<std::int32_t>> const output_table_in
  = svn::base::multi_list_table_init_in(audio_output_counts, audio_outputs_count);
  static inline std::vector<std::pair<std::int32_t, std::int32_t>> const input_table_out
    = svn::base::multi_list_table_init_out(audio_input_counts, audio_inputs_count);

  std::vector<base::audio_sample32> scratch;
  base::audio_sample32* output_buffer(std::int32_t output, std::int32_t index);
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

inline base::audio_sample32* 
audio_state::output_buffer(std::int32_t output, std::int32_t index)
{
  switch (output)
  {
  case audio_route_output::off: return nullptr;
  case audio_route_output::amp: return voice_amp.data();
  case audio_route_output::filter: return voice_filter[index].data();
  default: assert(false); return nullptr;
  }
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