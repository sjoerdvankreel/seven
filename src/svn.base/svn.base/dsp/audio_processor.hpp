#ifndef SVN_BASE_DSP_AUDIO_PROCESSOR_HPP
#define SVN_BASE_DSP_AUDIO_PROCESSOR_HPP

#include <svn.base/dsp/note_event.hpp>
#include <svn.base/dsp/input_buffer.hpp>
#include <svn.base/dsp/audio_sample.hpp>

#include <vector>
#include <cstdint>

namespace svn::base {

struct processor_type_t { enum value { synth, fx, count }; };
typedef processor_type_t::value processor_type;

class audio_processor
{
private:
  input_buffer _input;
  std::vector<note_event> _notes;
  std::vector<audio_sample> _audio;
  std::vector<audio_sample> _part_audio;
  std::vector<void*> _automation;
  std::vector<float> _automation_real;
  std::vector<std::int32_t> _automation_discrete;
private:
  float const _sample_rate;
  processor_type const _type;
  union param_value* const _state;
  struct runtime_topology const* const _topology;
private:
  void state_check();
  void automation_check(std::int32_t sample_count);
protected:
  virtual void 
  process_block(
    input_buffer const& input,
    audio_sample* audio,
    param_value* state) = 0;
public:
  audio_processor(
    processor_type type,
    struct runtime_topology const* topology,
    float sample_rate,
    std::int32_t max_sample_count,
    param_value* state);
public: 
  processor_type type() const;
  struct audio_sample const* process_block();
  input_buffer& prepare_block(std::int32_t sample_count);  
};

} // namespace svn::base
#endif // SVN_BASE_DSP_AUDIO_PROCESSOR_HPP