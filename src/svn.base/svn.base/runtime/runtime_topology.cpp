#include <svn.base/support/param_value.hpp>
#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_param.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

namespace svn::base {

runtime_topology::
runtime_topology(
  std::vector<runtime_part> const& parts,
  std::vector<runtime_param> const& params,
  std::vector<std::vector<std::int32_t>> const& bounds,
  std::int32_t real_count,
  std::int32_t discrete_count):
parts(parts), params(params), bounds(bounds),
real_count(real_count), discrete_count(discrete_count)
{
  assert(parts.size() > 0);
  assert(params.size() > 0);
  assert(bounds.size() > 0);
  assert(real_count + discrete_count == params.size());
}

void 
runtime_topology::init_defaults(param_value* state) const
{
  for (std::size_t p = 0; p < params.size(); p++)
    switch (params[p].descriptor->type)
    {
    case param_type::real: state[p].real = params[p].descriptor->default_.real; break;
    default: state[p].discrete = params[p].descriptor->default_.discrete; break;
    }
}

runtime_topology
runtime_topology::create(part_descriptor const* static_parts, std::int32_t count)
{
  std::int32_t real_count = 0;
  std::int32_t discrete_count = 0;
  std::vector<runtime_part> parts;
  std::vector<runtime_param> params;
  std::vector<std::vector<std::int32_t>> bounds;

  std::int32_t param_index = 0;
  for (std::int32_t t = 0; t < count; t++)
  {
    std::int32_t type_index = 0;
    bounds.push_back(std::vector<std::int32_t>());
    std::wstring type_name(static_parts[t].static_name.short_);
    for (std::int32_t i = 0; i < static_parts[t].part_count; i++)
    {
      bounds[t].push_back(param_index);
      std::wstring runtime_name = type_name;
      param_index += static_parts[t].param_count;
      if (static_parts[t].part_count > 1) runtime_name += std::wstring(L" ") + std::to_wstring(i + 1);
      parts.push_back(runtime_part(type_index++, runtime_name, &static_parts[t]));
    }
  }

  for (std::size_t part = 0; part < parts.size(); part++)
  {
    std::wstring part_name = parts[part].runtime_name;
    for (std::int32_t param = 0; param < parts[part].descriptor->param_count; param++)
    {
      auto const& descriptor = parts[part].descriptor->params[param];
      std::wstring runtime_name = part_name + L" " + descriptor.static_name.detail;
      params.push_back(runtime_param(part, runtime_name, &parts[part], &descriptor));
      if (descriptor.type == param_type::real) ++real_count;
      else ++discrete_count;
    }
  }

  return runtime_topology(parts, params, bounds, real_count, discrete_count);
}

} // namespace svn::base