#ifndef SVN_BASE_DSP_AUDIO_PROCESSOR_HPP
#define SVN_BASE_DSP_AUDIO_PROCESSOR_HPP

#include <svn.base/dsp/note_event.hpp>
#include <svn.base/dsp/block_input.hpp>
#include <svn.base/dsp/audio_sample.hpp>

#include <vector>
#include <cstdint>

namespace svn::base {

class audio_processor
{
private:
  block_input _input;
  std::vector<audio_sample> _audio;
  std::vector<note_event> _notes;
  std::vector<note_event*> _sample_notes;
  std::vector<std::int32_t> _sample_note_counts;
  std::vector<void*> _automation;
  std::vector<float> _automation_real;
  std::vector<std::int32_t> _automation_discrete;

  float const _sample_rate;
  union param_value* const _state;
  struct runtime_topology const* const _topology;

  void state_check();
  void automation_check(std::int32_t sample_count);

protected:
  virtual void
  process_block(block_input const& input,
    param_value* state, audio_sample* audio) = 0;

public:
  struct audio_sample const* process_block();
  struct runtime_topology const& topology() const;
  block_input& prepare_block(std::int32_t sample_count);

  virtual ~audio_processor() = default;
  audio_processor(
    struct runtime_topology const* topology, float sample_rate, 
    std::int32_t max_sample_count, param_value* state);
};

inline runtime_topology const& 
audio_processor::topology() const
{ return *_topology; }

} // namespace svn::base
#endif // SVN_BASE_DSP_AUDIO_PROCESSOR_HPP