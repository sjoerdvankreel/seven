#ifndef SVN_SYNTH_DSP_SUPPORT_HPP
#define SVN_SYNTH_DSP_SUPPORT_HPP

#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/dsp/automation_view.hpp>
#include <svn.synth/topology/topology.hpp>

#include <array>
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
  std::vector<base::audio_sample> output;
  std::array<std::vector<base::audio_sample>, oscillator_count> oscillator_output;
  std::array<std::vector<base::audio_sample>, voice_filter_count> voice_filter_output;
  audio_state(std::int32_t max_sample_count);
};

inline audio_state::
audio_state(std::int32_t max_sample_count): 
oscillator_output(), voice_filter_output()
{
  std::vector<base::audio_sample> audio(static_cast<std::size_t>(max_sample_count));
  output = audio;
  oscillator_output.fill(audio);
  voice_filter_output.fill(audio);
}

} // namespace svn::base
#endif // SVN_SYNTH_DSP_SUPPORT_HPP