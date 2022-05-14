#include <svn/topology/param_id.hpp>
#include <svn/topology/param_topology.hpp>

namespace svn {

param_topology const 
param_topologies[param_id::count] =
{
  { .type = param_type::real, .name = L"Lvl", .description = L"Level", .real = { .min = 0.0f, .max = 1.0f, .default_ = 1.0f } },
  { param_type::real, L"Dtn", L"Detune" },
  { param_type::real, L"Pan", L"Panning" },
  { param_type::real, L"Pw", L"Pulse width" },
  { param_type::real, L"Frq", L"Frequency" },
  { param_type::real, L"Res", L"Resonance" }
};

} // namespace svn