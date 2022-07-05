#ifndef SVN_BASE_DSP_AUDIO_PROCESSOR_HPP
#define SVN_BASE_DSP_AUDIO_PROCESSOR_HPP

#include <svn.base/dsp/io_types.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace svn::base {

class audio_processor
{
private:
  block_input _input;
  block_output _output;
  std::vector<note_event> _notes;
  std::vector<audio_sample32> _audio;

  float const _sample_rate;
  param_value* const _state;
  topology_info const* const _topology;

  // Of size total runtime parameter count in topology.
  // Pointers into _automation_buffer.
  std::vector<param_value*> _automation;
  // Contiguous array of size total input parameters in topology * max_sample_count.
  std::vector<param_value> _automation_buffer;
  // Output parameter values of size total output parameters in topology.
  std::vector<param_value> _output_param_buffer;

private:
  void transform_automation();
  void automation_check(std::int32_t sample_count);

public:
  virtual ~audio_processor() = default;
  audio_processor(
    topology_info const* topology, float sample_rate, 
    std::int32_t max_sample_count, param_value* state);

  float sample_rate() const { return _sample_rate; }
  topology_info const* topology() const { return _topology; }
  // Process block call overwrites real valued automation with actual range.
  block_output const& process_block();
  // Caller writes either discrete or normalized (0..1) values to automation.
  block_input& prepare_block(std::int32_t sample_count);

protected:
  virtual void process_block(block_input const& input, block_output& output) = 0;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_AUDIO_PROCESSOR_HPP