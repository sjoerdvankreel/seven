#ifndef SVN_SYNTH_DSP_SYNTHESIZER_HPP
#define SVN_SYNTH_DSP_SYNTHESIZER_HPP

#include <svn.synth/dsp/voice.hpp>
#include <svn.synth/topology/topology.hpp>
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
class synthesizer:
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
  // Last automation values of previous buffer, in case we need them on sample 0 the next round.
  std::vector<base::param_value> _last_automation_previous_block;

  // Voice management. 
  // If a voice is activated within the current buffer,
  // it is considered taken from the beginning of the buffer.
  // Similarly, if a voice is finished within the current buffer,
  // it is considered taken untill the end of the buffer.
  bool _voices_drained = false;
  std::array<voice, synth_polyphony> _voices;
  std::array<voice_state, synth_polyphony> _voice_states;

public:
  synthesizer(
    base::topology_info const* topology, float sample_rate,
    std::int32_t max_sample_count, base::param_value* state);

protected:
  void process_block(base::block_input const& input, base::block_output& output) override;

private:
  // Oldest voice is recycled first.
  // Toggle _voices_drained to indicate whether an earlier 
  // started, not-yet-finished voice needed to be recycled.
  void return_voice(std::int32_t voice);
  void setup_voice_release(note_event const& note, std::int32_t sample_count);
  void setup_voice(note_event const& note, std::int32_t sample_count, std::int64_t stream_position);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SYNTHESIZER_HPP