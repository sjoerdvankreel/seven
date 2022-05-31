#include <svn.base/dsp/dsp.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/static/topology.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

oscillator::
oscillator(float sample_rate):
_sample_rate(sample_rate)
{ assert(sample_rate > 0.0f); }

void
oscillator::process_block(
  block_input const& input, 
  audio_sample* audio, param_value* state)
{
  assert(state != nullptr);
  assert(audio != nullptr);

  for (std::int32_t s = 0; s < input.global.sample_count; s++)
  {
    for(std::uint32_t n = 0; n < input.note_count[s]; n++)
    {
      if (input.notes[s][n].midi == note_off) _midi_note = note_none;
      else if (input.notes[s][n].midi >= 0)
      {
        _midi_note = input.notes[s][n].midi;
        _frequency = 440.0f * std::pow(2.0f, (_midi_note - 69) / 12.0f);
      }
    }

    if(_midi_note >= 0)
    {
      float level = automate_real(input, oscillator_params, state, oscillator_param::level, s);
      float panning = automate_real(input, oscillator_params, state, oscillator_param::panning, s);
      float sample = level * std::sin(2.0f * std::numbers::pi * _phase);

      audio[s].left = (1.0f - panning) * sample;
      audio[s].right = panning * sample;
      _phase += _frequency / _sample_rate;
      _phase -= std::floor(_phase);
    }
  }
}

} // namespace svn::synth