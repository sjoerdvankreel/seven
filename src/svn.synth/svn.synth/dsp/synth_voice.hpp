#ifndef SVN_SYNTH_DSP_SYNTH_VOICE_HPP
#define SVN_SYNTH_DSP_SYNTH_VOICE_HPP

#include <svn.synth/dsp/lfo.hpp>
#include <svn.synth/dsp/filter.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.synth/dsp/envelope.hpp>
#include <svn.synth/dsp/amplitude.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/dsp/audio_state.hpp>
#include <svn.synth/topology/topology.hpp>

#include <array>
#include <cstdint>

namespace svn::synth {

struct voice_state
{
  // True if processing any audio during current buffer.
  bool in_use = false;
  // True if already released.
  bool released_previous_buffer = false;
  // True if to be released in current buffer.
  bool release_this_buffer = false;
  // Find voice from midi note.
  std::int32_t midi = -1;
  // Start position in stream, used for recycling oldest voice when running out.
  std::int64_t start_position_stream = -1;
  // Start position in current buffer, 0 if started in previous buffer.
  std::int32_t start_position_buffer = -1;
  // Used to stop tracking automation after release.
  std::int32_t release_position_buffer = -1;
};

// Single voice in polyphonic synth.
class synth_voice
{
private:
  float _velocity;
  amplitude _amplitude;
  base::topology_info const* _topology;
  std::array<lfo, lfo_count> _lfos;
  std::array<filter, filter_count> _filters;
  std::array<envelope, envelope_count> _envelopes;
  std::array<oscillator, oscillator_count> _oscillators;

public:
  synth_voice() = default;
  synth_voice(base::topology_info const* topology,
    float sample_rate, float velocity, std::int32_t midi_note);

  // With input and audio offset to the start of the voice within the current 
  // block. Release sample is nonnegative if voice is released within the current block.
  // Automation should be fixed to the last active value if this voice is released,
  // this is handled globally by the synth class. Returns true if voice ended.
  bool
  process_block(voice_input const& input, cv_state& cv, 
    audio_state& audio, std::int32_t release_sample, cpu_usage& usage);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_SYNTH_VOICE_HPP
