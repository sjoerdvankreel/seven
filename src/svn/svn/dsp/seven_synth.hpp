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
  float const _sample_rate;
  param_value* const _state;
  unit_generator _unit_generators[unit_count];

  input_buffer _input;
  std::vector<void*> _automation;
  std::vector<note_event> _notes;
  std::vector<audio_sample> _audio;
  std::vector<audio_sample> _part_audio;
  std::vector<std::vector<float>> _automation_real;
  std::vector<std::vector<std::int32_t>> _automation_discrete;
public:
  seven_synth(param_value* state, float sample_rate, std::int32_t max_sample_count);
public:
  input_buffer& input();
  audio_sample* process();
};

inline input_buffer&
seven_synth::input()
{ return _input; }

} // namespace svn
#endif // SVN_DSP_SEVEN_SYNTH_HPP