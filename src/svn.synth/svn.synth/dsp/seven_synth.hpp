#ifndef SVN_SYNTH_DSP_SEVEN_SYNTH_HPP
#define SVN_SYNTH_DSP_SEVEN_SYNTH_HPP

#include <svn.synth/dsp/synth_voice.hpp>
#include <svn.synth/dsp/voice_state.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.base/dsp/audio_processor.hpp>

#include <array>
#include <cstdint>

using namespace svn::base;

namespace svn::synth {

// Note: release = both note off and no longer tracking automation.
// Could be improved by differentiating note off and channel reuse using
// VST3 noteId, but this currently seems unsupported by my host of choice (renoise).
// Unfortunately this also means a note off event for a specific midi note means
// releasing all voices playing that note.
class seven_synth:
public base::audio_processor
{
private:
  std::vector<audio_sample> _voice_audio;
  std::vector<audio_sample> _voice_audio_scratch;
  // Of size total voice count.
  // Pointers into _automation_fixed_buffer.
  std::vector<base::param_value*> _automation_fixed;
  // Contiguous array of size total voice count * total param count.
  std::vector<base::param_value> _automation_fixed_buffer;

  // Voice management. 
  // If a voice is activated within the current buffer,
  // it is considered taken from the beginning of the buffer.
  // Similarly, if a voice is finished within the current buffer,
  // it is considered taken untill the end of the buffer.
  std::array<synth_voice, synth_polyphony> _voices;
  std::array<voice_state, synth_polyphony> _voice_states;

  // Oldest voice is recycled first.
  void return_voice(
    std::int32_t voice);
  // True if exhausted.
  bool setup_voice(
    struct note_event const& note, 
    std::int32_t sample_count, std::int64_t stream_position);
  void setup_voice_release(
    struct note_event const& note, std::int32_t sample_count);

protected:
  void
  process_block(
    base::block_input const& input,
    base::block_output& output) override;

public:
  seven_synth(
    struct base::runtime_topology const* topology, float sample_rate,
    std::int32_t max_sample_count, base::param_value* state);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SEVEN_SYNTH_HPP