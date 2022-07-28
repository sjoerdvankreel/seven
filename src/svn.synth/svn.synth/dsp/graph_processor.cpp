#include <svn.base/dsp/fft.hpp>
#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/lfo.hpp>
#include <svn.synth/dsp/filter.hpp>
#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/envelope.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/dsp/audio_state.hpp>
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
  case svn::synth::part_type::amplitude:
    return new svn::synth::amp_graph(topology);
  case svn::synth::part_type::lfo:
    return new svn::synth::lfo_graph(topology, part_index);
  case svn::synth::part_type::cv_route:
    return new svn::synth::cv_route_graph(topology, part_index);
  case svn::synth::part_type::envelope:
    return new svn::synth::envelope_graph(topology, part_index);
  case svn::synth::part_type::filter:
    assert(0 <= part_index && part_index < svn::synth::filter_count);
    switch (graph_type)
    {
    case svn::synth::filter_graph::impulse:
      return new svn::synth::filter_ir_graph(topology, part_index);
    case svn::synth::filter_graph::frequency:
      return new svn::synth::filter_fr_graph(topology, part_index);
    default:
      assert(false);
      return nullptr;
    }
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

param_value
disable_modulation(topology_info const* topology, std::int32_t rt_index, param_value value)
{
  for(std::int32_t cv = 0; cv < cv_route_count; cv++)
    if(rt_index == topology->param_bounds[part_type::cv_route][cv] + cv_route_param::on)
      return param_value(0);
  return value;
}

bool
lfo_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::lfo][part_index()];
  return begin <= runtime_param && runtime_param < begin + lfo_param::count;
}

void
lfo_graph::process_dsp_core(
  block_input const& input, base::cv_sample* output, float sample_rate, float bpm)
{
  double cv_time = 0.0f;
  lfo lfo(lfo_graph_rate);
  cv_state cv(topology(), input.sample_count);
  std::memset(output, 0, input.sample_count * sizeof(base::cv_sample));
  voice_input vinput = setup_graph_voice_input(input, topology());
  lfo.process_block(vinput, part_index(), cv, output, cv_time);
}

void
lfo_graph::dsp_to_plot(
  param_value const* state, std::vector<base::cv_sample> const& dsp,
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  plot.resize(dsp.size());
  bipolar = dsp[0].bipolar;
  for(std::size_t i = 0; i < dsp.size(); i++)
    plot[i] = dsp[i].bipolar? (dsp[i].value + 1.0f) * 0.5f: dsp[i].value;
}

std::int32_t
lfo_graph::sample_count(param_value const* state, float sample_rate, float bpm) const
{
  std::int32_t const cycles = 2;
  state_view view(topology(), state, part_type::lfo, part_index());
  float samples = view.get_real_dsp(lfo_param::period_time) * lfo_graph_rate;
  if(view.get_discrete(lfo_param::synced) != 0)
  {
    float timesig = lfo_timesig_values[view.get_discrete(lfo_param::period_sync)];
    samples = timesig_to_samples(lfo_graph_rate, bpm, timesig);
  }
  return static_cast<std::int32_t>(std::ceil(cycles * samples));
}  

param_value 
amp_graph::transform_param(std::int32_t rt_index, param_value value) const
{ return disable_modulation(topology(), rt_index, value); }

std::int32_t
amp_graph::sample_count(param_value const* state, float sample_rate, float bpm) const
{ return _env.sample_count(state, sample_rate, bpm); }

void
amp_graph::process_dsp_core(
  block_input const& input, base::cv_sample* output, float sample_rate, float bpm)
{ _env.process_dsp_core(input, output, sample_rate, bpm); }

bool
amp_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::amplitude][0];
  if(begin <= runtime_param && runtime_param < begin + amplitude_param::count) return true;
  return _env.needs_repaint(runtime_param);
}

void
amp_graph::dsp_to_plot(
  param_value const* state, std::vector<base::cv_sample> const& dsp,
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = false;
  plot.resize(dsp.size());
  state_view view(topology(), state, part_type::amplitude, 0);
  float amp = view.get_real_dsp(amplitude_param::level);
  for(std::size_t i = 0; i < dsp.size(); i++)
    plot[i] = (dsp[i].bipolar? (dsp[i].value + 1.0f) * 0.5f: dsp[i].value) * amp;
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
  std::int32_t result = static_cast<std::int32_t>(std::ceil(delay + attack + hold + decay + release));
  return result;
}

void
envelope_graph::process_dsp_core(
  block_input const& input, base::cv_sample* output, float sample_rate, float bpm)
{
  bool ended = false;
  float delay, attack, hold, decay, release;
  setup_stages(input.automation, bpm, delay, attack, hold, decay, release);
  std::int32_t release_sample = static_cast<std::int32_t>(std::ceil(delay + attack + hold + decay));
  envelope env(env_graph_rate);
  std::memset(output, 0, input.sample_count * sizeof(base::cv_sample));
  voice_input vinput = setup_graph_voice_input(input, topology());
  env.process_block(vinput, part_index(), output, release_sample, ended);
}

void
envelope_graph::setup_stages(param_value const* const* automation, float bpm, 
  float& delay, float& attack, float& hold, float& decay, float& release) const
{
  envelope env(env_graph_rate);
  base::automation_view view(topology(), nullptr, automation,
    topology()->input_param_count, topology()->input_param_count, 0, 1, 0, 1);
  automation_view env_view = view.rearrange_params(part_type::envelope, part_index());
  env.setup_stages(env_view, 0, bpm, delay, attack, hold, decay, release);
}

bool 
oscillator_spectrum_graph::needs_repaint(std::int32_t runtime_param) const
{ return _wave.needs_repaint(runtime_param); }

void 
oscillator_spectrum_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{ return _wave.process_dsp_core(input, output, sample_rate, bpm); }

param_value 
oscillator_spectrum_graph::transform_param(std::int32_t rt_index, param_value value) const
{ return _wave.transform_param(rt_index, value); }

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

param_value 
oscillator_wave_graph::transform_param(std::int32_t rt_index, param_value value) const
{ return disable_modulation(topology(), rt_index, value); }

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
  state_view view(topology(), state, part_type::oscillator, part_index());
  float cent = view.get_real_dsp(oscillator_param::cent);
  std::int32_t note = view.get_discrete(oscillator_param::note);
  std::int32_t octave = view.get_discrete(oscillator_param::octave);
  float frequency = note_to_frequency(12 * (octave + 1) + note + cent);
  return static_cast<std::int32_t>(std::ceil(cycles * sample_rate / frequency));
} 
 
void 
oscillator_wave_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{
  double cv_time = 0.0;
  oscillator osc(sample_rate, midi_note_c4);
  cv_state cv(topology(), input.sample_count);
  std::memset(output, 0, input.sample_count * sizeof(audio_sample32));
  voice_input vinput = setup_graph_voice_input(input, topology());
  osc.process_block(vinput, part_index(), cv, output, cv_time);
}

std::int32_t
filter_fr_graph::sample_count(
  param_value const* state, float sample_rate, float bpm) const
{ return _ir.sample_count(state, sample_rate, bpm); }

bool 
filter_fr_graph::needs_repaint(std::int32_t runtime_param) const
{ return _ir.needs_repaint(runtime_param); }

param_value 
filter_fr_graph::transform_param(std::int32_t rt_index, param_value value) const
{ return _ir.transform_param(rt_index, value); }

void 
filter_fr_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{ return _ir.process_dsp_core(input, output, sample_rate, bpm); }

// https://dsp.stackexchange.com/questions/20500/negative-values-of-the-fft
void
filter_fr_graph::dsp_to_plot(
  param_value const* state, std::vector<audio_sample32> const& dsp, 
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = false;
  _mono.clear();
  float max = 0.0f;
  for(std::size_t s = 0; s < dsp.size(); s++)
    _mono.push_back(dsp[s].mono());
  std::vector<std::complex<float>> const& fft = _fft.transform(_mono.data(), _mono.size());
  for(std::size_t i = 0; i < fft.size(); i++)
    max = std::max(max, std::abs(fft[i].real()));
  for (std::size_t i = 0; i < fft.size(); i++)
    plot.push_back(std::abs(fft[i].real()) / max);
}

param_value 
filter_ir_graph::transform_param(std::int32_t rt_index, param_value value) const
{ return disable_modulation(topology(), rt_index, value); }

std::int32_t
filter_ir_graph::sample_count(
  param_value const* state, float sample_rate, float bpm) const
{
  state_view view(topology(), state, part_type::filter, part_index());
  std::int32_t type = view.get_discrete(filter_param::type);
  std::int32_t length_ms = type == filter_type::state_var? 5: 50;
  return static_cast<std::int32_t>(std::ceil(length_ms * sample_rate / 1000.0f));
}

bool
filter_ir_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::filter][part_index()];
  return begin <= runtime_param && runtime_param < begin + oscillator_param::count;
}

void
filter_ir_graph::dsp_to_plot(
  param_value const* state, std::vector<base::audio_sample32> const& dsp, 
  std::vector<float>& plot, float sample_rate, bool& bipolar)
{
  bipolar = true;
  for (std::size_t s = 0; s < dsp.size(); s++)
    plot.push_back(std::clamp((dsp[s].mono() + 1.0f) * 0.5f, 0.0f, 1.0f));
} 

void 
filter_ir_graph::process_dsp_core(
  block_input const& input, base::audio_sample32* output, float sample_rate, float bpm)
{
  _audio_in.clear();
  _audio_in.resize(input.sample_count);
  _audio_in[0] = 1.0f;

  double cv_time = 0.0;
  cv_state cv(topology(), input.sample_count);
  std::memset(output, 0, input.sample_count * sizeof(audio_sample32));
  voice_input vinput = setup_graph_voice_input(input, topology());
  auto flt = std::make_unique<filter>(sample_rate, midi_note_c4);
  flt->process_block(vinput, part_index(), cv, _audio_in.data(), output, cv_time);
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
  state_view view(topology(), state, part_type::cv_route, part_index());
  float seconds = view.get_real_dsp(cv_route_param::plot_time);
  return static_cast<std::int32_t>(std::ceil(seconds * cv_route_graph_rate));
}

void
cv_route_graph::process_dsp_core(
  block_input const& input, float* output, float sample_rate, float bpm)
{
  bool ended = false;
  double cv_time = 0.0f;
  cv_state state(topology(), input.sample_count);
  voice_input vinput = setup_graph_voice_input(input, topology());
  for(std::int32_t i = 0; i < lfo_count; i++)
    lfo(cv_route_graph_rate).process_block(vinput, i, state, state.lfo[i].data(), cv_time);
  for (std::int32_t i = 0; i < envelope_count; i++)
    envelope(cv_route_graph_rate).process_block(vinput, i, state.envelope[i].data(), input.sample_count, ended);
  std::fill(state.velocity.begin(), state.velocity.begin() + input.sample_count, base::cv_sample({ cv_route_graph_velocity, false }));

  std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> output_table_out
    = zip_list_table_init_out(cv_route_output_counts, cv_route_output_target_counts, cv_route_output::count);
  automation_view automation = vinput.automation.rearrange_params(part_type::cv_route, part_index());
  std::int32_t target_id = automation.input_discrete(cv_route_param::plot_tgt, 0);
  auto param_ids = output_table_out[target_id];
  std::memset(output, 0, input.sample_count * sizeof(float));
  if (std::get<0>(param_ids) == -1 || std::get<2>(param_ids) == -1) return;

  float const* const* transformed_cv;
  std::int32_t rt_part_index = std::get<1>(param_ids);
  std::int32_t cv_route_output_id = std::get<0>(param_ids);
  std::int32_t cv_route_target = std::get<2>(param_ids);
  std::int32_t part_type = cv_route_part_mapping[cv_route_output_id];
  std::int32_t param_index = cv_route_param_mapping[cv_route_output_id][cv_route_target];
  automation_view automated_view = vinput.automation.rearrange_params(part_type, rt_part_index);
  state.transform_modulated(vinput, automated_view, static_cast<cv_route_output>(cv_route_output_id),
    rt_part_index, cv_route_param_mapping[cv_route_output_id], transformed_cv);
  for (std::int32_t i = 0; i < input.sample_count; i++)
    output[i] = automated_view.from_dsp(param_index, transformed_cv[param_index][i]);
}
 
} // namespace svn::synth