#include <svn.synth/static/topology.hpp>
#include <svn.base/static/item_name.hpp>

using namespace svn::base;

namespace svn::synth {

static item_name const
filter_types[filter_type::count] =
{
  { L"Lddr", L"Ladder" },
  { L"StVar", L"State variable" }
};

static item_name const
oscillator_types[oscillator_type::count] =
{
  { L"Blep", L"Bandlimited step" },
  { L"Blamp", L"Bandlimited ramp" },
  { L"Dsf", L"Discrete summation formulae" }
};

param_descriptor const 
output_params[output_param::count] =
{
  { { L"Clip", L"Clip" }, false },
  { { L"Voices", L"Voice count" }, L"", 0, 0, synth_polyphony }
};

part_descriptor const
part_descriptors[part_type::count] =
{
  { part_type::oscillator, { L"Osc", L"Oscillator" }, oscillator_count, oscillator_params, oscillator_param::count },
  { part_type::filter, { L"Filter", L"Filter" }, filter_count, filter_params, filter_param::count }
};

param_descriptor const
oscillator_params[oscillator_param::count] =
{
  { { L"On", L"Enabled" }, false },
  { { L"Type", L"Type" }, oscillator_types, oscillator_type::count },
  { { L"Lvl", L"Level" }, L"dB", 1.0, param_bounds::linear_unit(), param_bounds::decibel() },
  { { L"Dtn", L"Detune" }, L"Cent", 0, -50, 50 },
  { { L"Dt2", L"Detune2" }, L"Cent", 0.5, param_bounds::linear(-0.5, 0.5), param_bounds::linear(-50.0, 50.0) },
  { { L"Pan", L"Panning" }, L"%", 0.5, param_bounds::linear_unit(), param_bounds::linear(-100.0, 100.0) },
  { { L"Pw", L"Pulse width" }, L"%", 1.0, param_bounds::linear(0.0, 0.5), param_bounds::linear(0.0, 100.0) },
  { { L"Dcy", L"Decay" }, L"Sec", 0.0, param_bounds::linear_unit(), param_bounds::linear_unit() }
};

param_descriptor const
filter_params[filter_param::count] =
{
  { { L"On", L"Enabled" }, false },
  { { L"Type", L"Type" }, filter_types, filter_type::count },
  { { L"Frq", L"Frequency" }, L"Hz", 0.5, param_bounds::logarithmic(20.0, 20000.0, 2000.0), param_bounds::logarithmic(20.0, 20000.0, 2000.0) },
  { { L"Res", L"Resonance" }, L"", 0.0, param_bounds::linear_unit(), param_bounds::linear_unit() }
};

} // namespace svn::synth