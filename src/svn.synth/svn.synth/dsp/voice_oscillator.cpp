#include <svn.base/dsp/dsp.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.synth/dsp/voice_input.hpp>
#include <svn.synth/dsp/voice_oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

voice_oscillator::
voice_oscillator(
  float sample_rate, std::int32_t midi_note):
_sample_rate(sample_rate),
_midi_note(midi_note)
{
  assert(sample_rate > 0.0f);
  assert(0 <= midi_note && midi_note < 128);
}

void
voice_oscillator::process_block(
  voice_input const& input, audio_sample* audio)
{
  assert(audio != nullptr);
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio[s] = 0.0f;
    if(input.automation.get(voice_osc_param::on, s).discrete == 0) continue;
    float panning = input.automation.get(voice_osc_param::pan, s).real;
    float cent = input.automation.get(voice_osc_param::detune, s).real;
    std::int32_t note = input.automation.get(voice_osc_param::note, s).discrete;
    std::int32_t octave = input.automation.get(voice_osc_param::oct, s).discrete;
    float frequency = note_to_frequency(12 * (octave + 1) + note + cent + _midi_note - 60);
    float sample = std::sin(2.0f * std::numbers::pi * _phase);
    audio[s].left = (1.0f - panning) * sample;
    audio[s].right = panning * sample;
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth