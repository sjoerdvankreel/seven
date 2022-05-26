#ifndef SVN_SYNTH_DSP_SEVEN_SYNTH_HPP
#define SVN_SYNTH_DSP_SEVEN_SYNTH_HPP

#include <svn.synth/topology.hpp>
#include <svn.synth/oscillator.hpp>
#include <svn.base/dsp/input_buffer.hpp>

#include <vector>
#include <cstdint>

namespace svn::synth {

class seven_synth
{
private:
  base::input_buffer _input;
  std::vector<base::note_event> _notes;
  std::vector<base::audio_sample> _audio;
  std::vector<base::audio_sample> _part_audio;
  std::vector<void*> _automation;
  std::vector<float> _automation_real;
  std::vector<std::int32_t> _automation_discrete;
private:
  float const _sample_rate;
  union base::param_value* const _state;
  oscillator _oscillators[oscillator_count];
  struct base::runtime_topology const* const _topology;
private:
  void state_check();
  void automation_check(std::int32_t sample_count);
public:
  seven_synth(
    struct base::runtime_topology const* topology,
    float sample_rate, 
    std::int32_t max_sample_count,
    base::param_value* state);
public:
  struct base::audio_sample const* process_block();
  base::input_buffer& prepare_block(std::int32_t sample_count);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SEVEN_SYNTH_HPP