#include <svn/topology/param_id.hpp>
#include <svn/topology/param_topology.hpp>

namespace svn {

param_topology const 
param_topologies[param_id::count] =
{
  { param_type::real, L"Lvl", L"Level" },
  { param_type::real, L"Dtn", L"Detune" },
  { param_type::real, L"Pan", L"Panning" },
  { param_type::real, L"Pw", L"Pulse width" },
  { param_type::real, L"Frq", L"Frequency" },
  { param_type::real, L"Res", L"Resonance" }
};

} // namespace svn