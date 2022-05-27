#ifndef SVN_SYNTH_DSP_SEVEN_SYNTH_HPP
#define SVN_SYNTH_DSP_SEVEN_SYNTH_HPP

#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.base/dsp/audio_processor.hpp>

#include <array>
#include <cstdint>

namespace svn::synth {

class seven_synth:
public base::audio_processor
{
private:
  std::array<oscillator, oscillator_count> _oscillators;

protected:
  void
  process_block(
    base::input_buffer const& input,
    struct base::audio_sample* audio, 
    struct base::audio_sample* part_audio, 
    union base::param_value* state) override;

public:
  seven_synth(
    struct base::runtime_topology const* topology, float sample_rate,
    std::int32_t max_sample_count, base::param_value* state);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SEVEN_SYNTH_HPP