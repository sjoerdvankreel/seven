#ifndef SVN_DSP_SYNTH_HPP
#define SVN_DSP_SYNTH_HPP

#include <svn/dsp/unit.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/audio_sample.hpp>
#include <vector>

namespace svn {

class synth
{
  unit _units[unit_count];
  std::vector<audio_sample> _part_audio;
public:
  synth(std::int32_t max_sample_count);
  void process(input_buffer const& input, output_buffer& output);
};

} // namespace svn
#endif // SVN_DSP_SYNTH_HPP