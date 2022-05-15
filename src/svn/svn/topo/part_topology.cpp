#include <svn/topology/part_topology.hpp>

namespace svn {

part_topology const 
part_topologies[part_type::count] =
{
  { L"Osc", L"Oscillator", oscillator_count, param_id::oscillator_enabled, param_id::oscillator_pulse_width },
  { L"Flt", L"Filter", filter_count, param_id::filter_enabled, param_id::filter_resonance }
};

} // namespace svn