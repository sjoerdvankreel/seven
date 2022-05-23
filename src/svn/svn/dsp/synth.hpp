#ifndef SVN_DSP_SYNTH_HPP
#define SVN_DSP_SYNTH_HPP

#include <svn/dsp/unit.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/input_buffer.hpp>
#include <svn/support/param_value.hpp>
#include <vector>

namespace svn {

class synth
{
  unit _units[unit_count];
  float const _sample_rate;
  param_value* const _state;

  input_buffer _input;
  std::vector<void*> _automation;
  std::vector<note_event> _notes;
  std::vector<audio_sample> _audio;
  std::vector<audio_sample> _part_audio;
  std::vector<std::vector<float>> _automation_real;
  std::vector<std::vector<std::int32_t>> _automation_discrete;
public:
  synth(param_value* state, float sample_rate, std::int32_t max_sample_count);
public:
  input_buffer& input();
  audio_sample* process();
};

inline input_buffer&
synth::input()
{ return _input; }

} // namespace svn
#endif // SVN_DSP_SYNTH_HPP