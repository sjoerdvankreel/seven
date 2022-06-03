#ifndef SVN_SYNTH_DSP_SEVEN_SYNTH_HPP
#define SVN_SYNTH_DSP_SEVEN_SYNTH_HPP

#include <svn.synth/dsp/synth_voice.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/dsp/automation_buffer.hpp>

#include <array>
#include <cstdint>

using namespace svn::base;

namespace svn::synth {

// Note: release = note off, but still actively tracking automation until deactivation.
// Deactivation = channel is now occupied by another voice, no longer actively 
// tracking automation but automation is instead fixed to the last active value.
class seven_synth:
public base::audio_processor
{
private:
  // Same as synth automation when voice is active,
  // filled with automation state at moment of deactivation otherwise.
  automation_buffer _automation_scratch;
  std::vector<audio_sample> _audio_scratch;
  std::array<synth_voice, synth_polyphony> _voices;

protected:
  void
  process_block(
    base::block_input const& input,
    struct base::audio_sample* audio) override;

public:
  seven_synth(
    struct base::runtime_topology const* topology, float sample_rate,
    std::int32_t max_sample_count, base::param_value* state);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SEVEN_SYNTH_HPP