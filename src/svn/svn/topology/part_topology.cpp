#include <svn/topology/part_id.hpp>
#include <svn/topology/part_topology.hpp>

namespace svn {

part_topology const 
part_topologies[part_id::count] =
{
  { param_id::oscillator_level, param_id::oscillator_pulse_width, L"Osc", L"Oscillator" },
  { param_id::filter_frequency, param_id::filter_resonance, L"Flt", L"Filter" }
};

} // namespace svn