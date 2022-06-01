#ifndef SVN_SYNTH_DSP_SEVEN_SYNTH_HPP
#define SVN_SYNTH_DSP_SEVEN_SYNTH_HPP

#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.base/dsp/audio_processor.hpp>

#include <array>
#include <cstdint>

using namespace svn::base;

namespace svn::synth {

class seven_synth:
public base::audio_processor
{
private:
  std::vector<audio_sample> _part_audio;
  std::array<oscillator, oscillator_count> _oscillators;

protected:
  void
  process_block(
    base::block_input const& input,
    union base::param_value* state,
    struct base::audio_sample* audio) override;

public:
  seven_synth(
    struct base::runtime_topology const* topology, float sample_rate,
    std::int32_t max_sample_count, base::param_value* state);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SEVEN_SYNTH_HPP