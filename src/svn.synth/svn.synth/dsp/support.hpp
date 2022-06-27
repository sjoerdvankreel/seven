#ifndef SVN_SYNTH_DSP_SUPPORT_HPP
#define SVN_SYNTH_DSP_SUPPORT_HPP

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

// Internal audio outputs.
struct audio_state
{
  std::vector<base::audio_sample32> voice_amp;
  std::array<std::vector<base::audio_sample32>, oscillator_count> oscillator;
  std::array<std::vector<base::audio_sample32>, voice_filter_count> voice_filter;
  
  base::audio_sample32 mix(
    svn::base::automation_view const& automation,
    audio_route_output output, std::int32_t index, std::int32_t sample) const;

  audio_state(std::int32_t max_sample_count);
  base::audio_sample32* output_buffer(audio_route_output output, std::int32_t index);
  base::audio_sample32 const* input_buffer(audio_route_input input, std::int32_t index) const;
};

inline audio_state::
audio_state(std::int32_t max_sample_count): 
oscillator(), voice_filter()
{
  std::vector<base::audio_sample32> audio(static_cast<std::size_t>(max_sample_count));
  voice_amp = audio;
  oscillator.fill(audio);
  voice_filter.fill(audio);
}

inline base::audio_sample32* 
audio_state::output_buffer(audio_route_output output, std::int32_t index)
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
audio_state::input_buffer(audio_route_input input, std::int32_t index) const
{
  switch (input)
  {
  case audio_route_input::off: return nullptr;
  case audio_route_input::osc: return oscillator[index].data();
  case audio_route_input::filter: return voice_filter[index].data();
  default: assert(false); return nullptr;
  }
}

inline svn::base::audio_sample32 
audio_state::mix(
  svn::base::automation_view const& automation, 
  audio_route_output output, std::int32_t index, std::int32_t sample) const
{
  svn::base::audio_sample32 result = { 0.0f };
  for (std::int32_t i = 0; i < audio_route_count; i++)
  {
    std::int32_t out = automation.get(audio_route_param::in1 * i + 1, sample).discrete;
    //if(out != )
    std::int32_t in = automation.get(audio_route_param::in1 * i, sample).discrete;

    float amt = automation.get(audio_route_param::in1 * i + 2, sample).real;
  }
  return result;
}

} // namespace svn::base
#endif // SVN_SYNTH_DSP_SUPPORT_HPP