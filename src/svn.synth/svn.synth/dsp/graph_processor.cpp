#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/spectrum_analyzer.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/dsp/graph_processor.hpp>
#include <svn.synth/topology/topology.hpp>
#include <cassert>

using namespace svn::base;

svn::base::graph_processor*
svn_create_graph_processor(svn::base::topology_info const* topology,
  std::int32_t part_type, std::int32_t graph_type, std::int32_t part_index)
{
  switch (part_type)
  {
  case svn::synth::part_type::voice_filter:
    return new svn::synth::oscillator_wave_graph(topology, 0);
  case svn::synth::part_type::oscillator:
    assert(0 <= part_index && part_index < svn::synth::oscillator_count);
    switch (graph_type)
    {
    case svn::synth::oscillator_graph::wave:
      return new svn::synth::oscillator_wave_graph(topology, part_index);
    case svn::synth::oscillator_graph::spectrum:
      return new svn::synth::oscillator_spectrum_graph(topology, part_index);
    default:
      assert(false);
      return nullptr;
    }
  default:
    assert(false);
    return nullptr;
  }   
}

namespace svn::synth {

std::int32_t
oscillator_spectrum_graph::audio_sample_count(
  param_value const* state, float sample_rate) const
{
  std::int32_t one_second = std::ceil(sample_rate);
  return static_cast<std::int32_t>(one_second);
}

void 
oscillator_spectrum_graph::process_audio_core(
  block_input const& input, block_output& output, float sample_rate)
{
  oscillator_wave_graph wave(topology(), part_index());
  return wave.process_audio_core(input, output, sample_rate);
}

bool 
oscillator_spectrum_graph::needs_repaint(std::int32_t runtime_param) const
{
  oscillator_wave_graph wave(topology(), part_index());
  return wave.needs_repaint(runtime_param);
}

void
oscillator_spectrum_graph::audio_to_plot(
  std::vector<audio_sample> const& audio, std::vector<float>& plot, float sample_rate)
{
  _mono.clear();
  for(std::size_t s = 0; s < audio.size(); s++)
    _mono.push_back(audio[s].mono());
  float const* spectrum = _analyzer.analyze(_mono.data(), _mono.size(), sample_rate);
  for(std::size_t i = 0; i < spectrum_analyzer::bucket_count; i++)
    plot.push_back(spectrum[i]);
}

bool 
oscillator_wave_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::oscillator][part_index()];
  return begin <= runtime_param && runtime_param < begin + oscillator_param::count;
}

void
oscillator_wave_graph::audio_to_plot(
  std::vector<audio_sample> const& audio, std::vector<float>& plot, float sample_rate)
{
  for (std::size_t s = 0; s < audio.size(); s++)
    plot.push_back((audio[s].mono() + 1.0f) * 0.5f);
}

std::int32_t 
oscillator_wave_graph::audio_sample_count(
  param_value const* state, float sample_rate) const
{
  std::int32_t const cycles = 2;
  std::int32_t begin = topology()->param_bounds[part_type::oscillator][part_index()];
  float cent = state[begin + oscillator_param::cent].real;
  std::int32_t note = state[begin + oscillator_param::note].discrete;
  std::int32_t octave = state[begin + oscillator_param::oct].discrete;
  float frequency = note_to_frequency(12 * (octave + 1) + note + cent);
  return static_cast<std::int32_t>(std::ceil(cycles * sample_rate / frequency));
}

void 
oscillator_wave_graph::process_audio_core(
  block_input const& input, block_output& output, float sample_rate)
{
  voice_input vinput;  
  vinput.bpm = input.bpm;
  vinput.sample_count = input.sample_count;
  vinput.stream_position = input.stream_position;
  vinput.automation = automation_view(
    topology(), nullptr, input.automation, topology()->input_param_count, 
    topology()->input_param_count, 0, input.sample_count, 0, input.sample_count);
  vinput.automation = vinput.automation.rearrange_params(part_type::oscillator, part_index());
  oscillator osc(sample_rate, midi_note_c4);
  osc.process_block(vinput, output.audio);
}
 
} // namespace svn::synth