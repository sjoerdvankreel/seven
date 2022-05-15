#include <svn/topology/param_id.hpp>
#include <svn/topology/param_lists.hpp>
#include <svn/topology/param_topology.hpp>

namespace svn {

param_topology::
param_topology(wchar_t const* name, wchar_t const* description, bool default_):
type(param_type::toggle), name(name), description(description), bounds()
{ bounds.toggle = { default_ }; }

param_topology::
param_topology(wchar_t const* name, wchar_t const* description, float default_):
type(param_type::toggle), name(name), description(description), bounds()
{ bounds.real = { default_ }; }

param_topology::
param_topology(wchar_t const* name, wchar_t const* description, param_item const* items, std::int32_t count):
type(param_type::list), name(name), description(description), bounds()
{ bounds.list = { items, count }; }

param_topology::
param_topology(wchar_t const* name, wchar_t const* description, std::int32_t min, std::int32_t max, std::int32_t default_):
type(param_type::list), name(name), description(description), bounds()
{ bounds.discrete = { min, max, default_ }; }

param_topology const 
param_topologies[param_id::count] =
{
  { L"On", L"Enabled", false },
  { L"Type", L"Type", oscillator_types, oscillator_type::count },
  { L"Lvl", L"Level", 1.0f },
  { L"Dtn", L"Detune", - 50, 50, 0 },
  { L"Pan", L"Panning", 0.5f },
  { L"Pw", L"Pulse width", 1.0f },
  
  { L"On", L"Enabled", false },
  { L"Type", L"Type", filter_types, filter_type::count },
  { L"Frq", L"Frequency", 0.5f },
  { L"Res", L"Resonance", 0.0f }
};

} // namespace svn