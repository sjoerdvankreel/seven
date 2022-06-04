#ifndef SVN_SYNTH_DSP_SYNTH_VOICE_HPP
#define SVN_SYNTH_DSP_SYNTH_VOICE_HPP

#include <svn.synth/static/topology.hpp>
#include <svn.synth/dsp/voice_oscillator.hpp>
#include <svn.base/dsp/audio_processor.hpp>

#include <array>
#include <cstdint>

using namespace svn::base;

namespace svn::synth {

class synth_voice
{
private:
  struct base::runtime_topology const* _topology;
  std::array<voice_oscillator, oscillator_count> _oscillators;

public:
  // With input and audio offset to the start of the voice within the current 
  // block. Returns total number of samples rendered, which equals input 
  // sample count if the voice did not finish within the current block. Release 
  // sample is nonnegative if voice is released within the current block.
  // Automation should be fixed to the last active value if this voice is released,
  // this is handled globally by the synth class.
  std::int32_t
  process_block(
    base::block_input const& input,
    struct base::audio_sample* audio_scratch,
    struct base::audio_sample* audio,
    std::int32_t release_sample);

  synth_voice() = default;
  synth_voice(
    struct base::runtime_topology const* topology,
    float sample_rate, float frequency, float velocity);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SYNTH_VOICE_HPP
