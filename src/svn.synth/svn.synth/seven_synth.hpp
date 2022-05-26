#ifndef SVN_SYNTH_DSP_SEVEN_SYNTH_HPP
#define SVN_SYNTH_DSP_SEVEN_SYNTH_HPP

#include <svn.synth/topology.hpp>
#include <svn.synth/oscillator.hpp>
#include <svn.base/dsp/audio_processor.hpp>
#include <cstdint>

namespace svn::synth {

class seven_synth:
public base::audio_processor
{
private:
  oscillator _oscillators[oscillator_count];
public:
  seven_synth(
    struct base::runtime_topology const* topology, float sample_rate,
    std::int32_t max_sample_count, base::param_value* state);
protected:
  void
  process_block(
    struct base::runtime_topology const& topology, 
    base::input_buffer const& input,
    struct base::audio_sample* audio, 
    struct base::audio_sample* part_audio, 
    union base::param_value* state) override;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SEVEN_SYNTH_HPP