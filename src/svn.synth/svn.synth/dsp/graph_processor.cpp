#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/spectrum_analyzer.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/envelope.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.synth/dsp/voice_lfo.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/dsp/audio_state.hpp>
#include <svn.synth/dsp/voice_filter.hpp>
#include <svn.synth/dsp/graph_processor.hpp>

#include <memory>
#include <cassert>

using namespace svn::base; 

svn::base::graph_processor_base*
svn_create_graph_processor(svn::base::topology_info const* topology,
  std::int32_t part_type, std::int32_t graph_type, std::int32_t part_index)
{
  switch (part_type)
  {
  case svn::synth::part_type::cv_route:
    return new svn::synth::cv_route_graph(topology);
  case svn::synth::part_type::envelope:
    return new svn::synth::envelope_graph(topology, part_index);
  case svn::synth::part_type::voice_lfo:
    return new svn::synth::voice_lfo_graph(topology, part_index);
  case svn::synth::part_type::voice_filter:
    return new svn::synth::voice_filter_ir_graph(topology, part_index);
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

static voice_input
setup_graph_voice_input(
  block_input const& input, topology_info const* topology)
{
  voice_input result;
  result.bpm = input.bpm;
  result.sample_count = input.sample_count;
  result.stream_position = input.stream_position;
  result.automation = automation_view(
    topology, nullptr, input.automation, topology->input_param_count,
    topology->input_param_count, 0, input.sample_count, 0, input.sample_count);
  return result;
}

bool
voice_lfo_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::voice_lfo][part_index()];
  return begin <= runtime_param && runtime_param < begin + voice_lfo_param::count;
}

void
voice_lfo_graph::process_dsp_core(
  block_input const& input, base::cv_sample* output, float sample_rate, float bpm)
{
  voice_lfo lfo(lfo_graph_rate);
  std::memset(output, 0, input.sample_count * sizeof(base::cv_sample));
  voice_input vinput = setup_graph_voice_input(input, topology());
  lfo.process_block(vinput, part_index(), output);
}

void
voice_lfo_graph::dsp_to_plot(
  param_value const* state, std::vector<base::cv_sample> const& dsp,
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  plot.resize(dsp.size());
  bipolar = dsp[0].bipolar;
  for(std::size_t i = 0; i < dsp.size(); i++)
    plot[i] = dsp[i].bipolar? (dsp[i].value + 1.0f) * 0.5f: dsp[i].value;
}

std::int32_t
voice_lfo_graph::sample_count(param_value const* state, float sample_rate, float bpm) const
{
  std::int32_t const cycles = 2;
  std::int32_t begin = topology()->param_bounds[part_type::voice_lfo][part_index()];  
  float samples = lfo_graph_rate / state[begin + voice_lfo_param::freq_time].real;
  if(cv_kind_is_synced(state[begin + voice_lfo_param::kind].discrete))
  {
    float timesig = voice_lfo_timesig_values[state[begin + voice_lfo_param::freq_sync].discrete];
    samples = timesig_to_samples(lfo_graph_rate, bpm, timesig);
  }
  return static_cast<std::int32_t>(std::ceil(cycles * samples));
}

bool
envelope_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::envelope][part_index()];
  return begin <= runtime_param && runtime_param < begin + envelope_param::count;
}

void
envelope_graph::dsp_to_plot(
  param_value const* state, std::vector<base::cv_sample> const& dsp,
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  plot.resize(dsp.size());
  bipolar = dsp[0].bipolar;
  for (std::size_t i = 0; i < dsp.size(); i++)
    plot[i] = dsp[i].bipolar ? (dsp[i].value + 1.0f) * 0.5f : dsp[i].value;
}

std::int32_t 
envelope_graph::sample_count(param_value const* state, float sample_rate, float bpm) const
{ 
  float delay, attack, hold, decay, release;
  std::vector<param_value const*> automation(
    static_cast<std::size_t>(topology()->input_param_count));
  for (std::int32_t i = 0; i < topology()->input_param_count; i++)
    automation[i] = &state[i];
  setup_stages(automation.data(), bpm, delay, attack, hold, decay, release);
  return static_cast<std::int32_t>(std::ceil(delay + attack + hold + decay + release));
}

void
envelope_graph::process_dsp_core(
  block_input const& input, base::cv_sample* output, float sample_rate, float bpm)
{
  float delay, attack, hold, decay, release;
  setup_stages(input.automation, bpm, delay, attack, hold, decay, release);
  std::int32_t release_sample = static_cast<std::int32_t>(std::ceil(delay + attack + hold + decay));
  envelope env(env_graph_rate);
  std::memset(output, 0, input.sample_count * sizeof(base::cv_sample));
  voice_input vinput = setup_graph_voice_input(input, topology());
  env.process_block(vinput, part_index(), output, release_sample);
}

void
envelope_graph::setup_stages(param_value const* const* automation, float bpm, 
  float& delay, float& attack, float& hold, float& decay, float& release) const
{
  envelope env(env_graph_rate);
  base::automation_view view(topology(), nullptr, automation,
    topology()->input_param_count, topology()->input_param_count, 0, 1, 0, 1);
  env.setup_stages(view.rearrange_params(part_type::envelope, part_index()),
    0, bpm, delay, attack, hold, decay, release);
}

bool 
oscillator_spectrum_graph::needs_repaint(std::int32_t runtime_param) const
{ return _wave.needs_repaint(runtime_param); }

void 
oscillator_spectrum_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{ return _wave.process_dsp_core(input, output, sample_rate, bpm); }

std::int32_t
oscillator_spectrum_graph::sample_count(
  param_value const* state, float sample_rate, float bpm) const
{ return static_cast<std::int32_t>(std::ceil(sample_rate)); }

void
oscillator_spectrum_graph::dsp_to_plot(
  param_value const* state, std::vector<audio_sample32> const& dsp, 
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = false;
  _mono.clear();
  for(std::size_t s = 0; s < dsp.size(); s++)
    _mono.push_back(dsp[s].mono());
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
oscillator_wave_graph::dsp_to_plot(
  param_value const* state, std::vector<base::audio_sample32> const& dsp, 
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = true;
  for (std::size_t s = 0; s < dsp.size(); s++)
    plot.push_back((dsp[s].mono() + 1.0f) * 0.5f);
}

std::int32_t 
oscillator_wave_graph::sample_count(
  param_value const* state, float sample_rate, float bpm) const
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
oscillator_wave_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{
  cv_state cv(input.sample_count);
  oscillator osc(sample_rate, midi_note_c4);
  std::memset(output, 0, input.sample_count * sizeof(audio_sample32));
  voice_input vinput = setup_graph_voice_input(input, topology());
  osc.process_block(vinput, part_index(), cv, output);
}

std::int32_t
voice_filter_ir_graph::sample_count(
  param_value const* state, float sample_rate, float bpm) const
{
  std::int32_t begin = topology()->param_bounds[part_type::voice_filter][part_index()];
  std::int32_t type = state[begin + voice_filter_param::type].discrete;
  std::int32_t length_ms = type == voice_filter_type::state_var? 5: 50;
  return static_cast<std::int32_t>(std::ceil(length_ms * sample_rate / 1000.0f));
}

bool
voice_filter_ir_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::voice_filter][part_index()];
  return begin <= runtime_param && runtime_param < begin + oscillator_param::count;
}

void
voice_filter_ir_graph::dsp_to_plot(
  param_value const* state, std::vector<base::audio_sample32> const& dsp, 
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = true;
  for (std::size_t s = 0; s < dsp.size(); s++)
    plot.push_back(std::clamp((dsp[s].mono() + 1.0f) * 0.5f, 0.0f, 1.0f));
}

void 
voice_filter_ir_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{
  _audio_in.clear();
  _audio_in.resize(input.sample_count);
  _audio_in[0] = 1.0f;

  std::memset(output, 0, input.sample_count * sizeof(audio_sample32));
  voice_input vinput = setup_graph_voice_input(input, topology());
  auto filter = std::make_unique<voice_filter>(sample_rate, midi_note_c4);
  filter->process_block(vinput, part_index(), _audio_in.data(), output);
}

bool
cv_route_graph::needs_repaint(std::int32_t runtime_param) const
{ return true; } 

void
cv_route_graph::dsp_to_plot(
  param_value const* state, std::vector<float> const& dsp, 
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = false;
  plot.resize(dsp.size());
  std::copy(dsp.begin(), dsp.end(), plot.begin());
}

std::int32_t
cv_route_graph::sample_count(param_value const* state, float sample_rate, float bpm) const
{ 
  std::int32_t begin = topology()->param_bounds[part_type::cv_route][0];
  float seconds = state[begin + cv_route_param::plot_time].real;
  return static_cast<std::int32_t>(std::ceil(seconds * cv_route_graph_rate));
}

void
cv_route_graph::process_dsp_core(
  block_input const& input, float* output, float sample_rate, float bpm)
{
  cv_state state(input.sample_count);  
  voice_input vinput = setup_graph_voice_input(input, topology());
  for(std::int32_t i = 0; i < voice_lfo_count; i++)
    voice_lfo(cv_route_graph_rate).process_block(vinput, i, state.voice_lfo[i].data());
  for (std::int32_t i = 0; i < envelope_count; i++)
    envelope(cv_route_graph_rate).process_block(vinput, i, state.envelope[i].data(), input.sample_count);

  std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> output_table_out
    = zip_list_table_init_out(cv_output_counts, cv_output_target_counts, cv_route_output::count);
  automation_view automation = vinput.automation.rearrange_params(part_type::cv_route, 0);
  std::int32_t target_id = automation.get(cv_route_param::plot_tgt, 0).discrete;
  auto param_ids = output_table_out[target_id];
  std::memset(output, 0, input.sample_count * sizeof(float));
  if (std::get<0>(param_ids) == -1 || std::get<2>(param_ids) == -1) return;

  std::int32_t rt_part_index = std::get<1>(param_ids);
  std::int32_t cv_route_output_id = std::get<0>(param_ids);
  std::int32_t cv_route_target = std::get<2>(param_ids);
  std::int32_t part_type = cv_route_part_mapping[cv_route_output_id];
  std::int32_t param_index = cv_route_param_mapping[cv_route_output_id][cv_route_target];
  automation_view automated_view = vinput.automation.rearrange_params(part_type, rt_part_index);
  float const* const* modulated = state.modulate(vinput, automated_view, 
    cv_route_param_mapping[cv_route_output_id], static_cast<cv_route_output>(cv_route_output_id), rt_part_index);
  for (std::int32_t i = 0; i < input.sample_count; i++)
    output[i] = modulated[param_index][i];
}
 
} // namespace svn::synth