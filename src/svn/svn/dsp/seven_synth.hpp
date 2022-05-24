#ifndef SVN_DSP_SEVEN_SYNTH_HPP
#define SVN_DSP_SEVEN_SYNTH_HPP

#include <svn/dsp/unit_generator.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/input_buffer.hpp>
#include <svn/support/param_value.hpp>
#include <vector>

namespace svn {

class seven_synth
{
private:
  float const _sample_rate;
  param_value* const _state;
  unit_generator _unit_generators[unit_count];
private:
  input_buffer _input;
  std::vector<void*> _automation;
  std::vector<note_event> _notes;
  std::vector<audio_sample> _audio;
  std::vector<audio_sample> _part_audio;
  std::vector<std::vector<float>> _automation_real;
  std::vector<std::vector<std::int32_t>> _automation_discrete;
private:
  void state_check();
  void automation_check(std::int32_t sample_count);
public:
  seven_synth(param_value* state, float sample_rate, std::int32_t max_sample_count);
public:
  audio_sample* process_block();
  input_buffer& prepare_block(std::int32_t sample_count);
};

} // namespace svn
#endif // SVN_DSP_SEVEN_SYNTH_HPP