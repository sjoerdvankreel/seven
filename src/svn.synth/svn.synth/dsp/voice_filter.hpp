#ifndef SVN_SYNTH_VOICE_FILTER_HPP
#define SVN_SYNTH_VOICE_FILTER_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/dsp/delay_buffer.hpp>
#include <svn.base/dsp/audio_sample.hpp>

#include <cmath>
#include <cassert>

namespace svn::synth {

// 5ms at 384kHz.
inline std::int32_t constexpr 
comb_filter_max_samples = 1920;

struct stvar_state
{
  float kbd_track_base;
  base::audio_sample64 ic1eq;
  base::audio_sample64 ic2eq;
};

struct comb_state
{
  base::delay_buffer<base::audio_sample32, comb_filter_max_samples> input;
  base::delay_buffer<base::audio_sample32, comb_filter_max_samples> output;
};

class voice_filter
{
  float _sample_rate;
  comb_state _comb;
  stvar_state _state_var;
private:
  base::audio_sample32 process_comb(
    svn::base::automation_view const& automation,
    svn::base::audio_sample32 const* source,
    std::int32_t sample);
  base::audio_sample32 process_state_variable(
    svn::base::automation_view const& automation,
    svn::base::audio_sample32 const* source,
    std::int32_t sample);
public:
  voice_filter() = default;
  voice_filter(float sample_rate, std::int32_t midi_note);
  void process_block(voice_input const& input, audio_state const& state, base::audio_sample32* output);
};

} // namespace svn::synth
#endif // SVN_SYNTH_VOICE_FILTER_HPP