#ifndef SVN_SYNTH_VOICE_FILTER_HPP
#define SVN_SYNTH_VOICE_FILTER_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/delay_buffer.hpp>
#include <svn.base/dsp/audio_sample.hpp>

#include <cassert>

namespace svn::synth {

struct stvar_state
{
  base::audio_sample64 ic1eq;
  base::audio_sample64 ic2eq;
};

class voice_filter
{
  float _sample_rate;
  stvar_state _state_var;
  std::int32_t _midi_note;
private:
  base::audio_sample32 process_state_variable(
    svn::base::automation_view const& automation,
    svn::base::audio_sample32 const* source,
    std::int32_t sample);
public:
  voice_filter() = default;
  voice_filter(float sample_rate, std::int32_t midi_note): _state_var(), _sample_rate(sample_rate), _midi_note(midi_note) {}
  void process_block(voice_input const& input, base::audio_sample32 const* source, base::audio_sample32* output);
};

} // namespace svn::synth
#endif // SVN_SYNTH_VOICE_FILTER_HPP