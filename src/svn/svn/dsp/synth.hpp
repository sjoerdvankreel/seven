#ifndef SVN_DSP_SYNTH_HPP
#define SVN_DSP_SYNTH_HPP

#include <svn/dsp/unit.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/event_buffer.hpp>
#include <vector>

namespace svn {

class synth
{
  input_buffer _input;
  output_buffer _output;
  unit _units[unit_count];
  std::vector<audio_sample> _part_audio;
  std::vector<note_event> _input_notes;
  std::vector<audio_sample> _output_audio;
  std::vector<param_value> _output_params;
  std::vector<void*> _automation;
  std::vector<std::vector<float>> _automation_real;
  std::vector<std::vector<std::int32_t>> _automation_discrete;
public:
  synth(std::int32_t max_sample_count);
public:
  input_buffer& input();
  output_buffer process();
};

inline input_buffer&
synth::input()
{ return _input; }

} // namespace svn
#endif // SVN_DSP_SYNTH_HPP