#include <svn.base/support/param_value.hpp>
#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_param.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

namespace svn::base {

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

std::unique_ptr<runtime_topology>
runtime_topology::create(
  part_descriptor const* static_parts, std::int32_t part_count, 
  param_descriptor const* output_params, std::int32_t output_param_count,
  std::int32_t max_note_events, std::int32_t max_ui_height, std::int32_t const* ui_order)
{
  assert(max_note_events >= 0);
  assert(output_param_count >= 0);
  assert(output_params == nullptr || output_param_count > 0);
  assert(ui_order == nullptr && max_ui_height == 0 || ui_order != nullptr && max_ui_height > 0);

  auto result = std::make_unique<runtime_topology>();
  result->ui_order = ui_order;
  result->max_note_events = max_note_events;
  result->output_params = output_params;
  result->max_ui_height = max_ui_height;
  result->output_param_count = output_param_count;

  std::int32_t param_index = 0;
  for (std::int32_t t = 0; t < part_count; t++)
  {
    std::int32_t type_index = 0;
    result->bounds.push_back(std::vector<std::int32_t>());
    std::wstring type_name(static_parts[t].static_name.short_);
    for (std::int32_t i = 0; i < static_parts[t].part_count; i++)
    {
      result->bounds[t].push_back(param_index);
      std::wstring runtime_name = type_name;
      param_index += static_parts[t].param_count;
      if (static_parts[t].part_count > 1) runtime_name += std::wstring(L" ") + std::to_wstring(i + 1);
      result->parts.push_back(runtime_part(type_index++, runtime_name, &static_parts[t]));
    }
  }

  for (std::size_t part = 0; part < result->parts.size(); part++)
  {
    std::wstring part_name = result->parts[part].runtime_name;
    for (std::int32_t param = 0; param < result->parts[part].descriptor->param_count; param++)
    {
      auto const& descriptor = result->parts[part].descriptor->params[param];
      std::wstring runtime_name = part_name + L" " + descriptor.static_name.detail;
      result->params.push_back(runtime_param(part, runtime_name, &result->parts[part], &descriptor));
    }
  }

  assert(result->parts.size() > 0);
  assert(result->params.size() > 0);
  assert(result->bounds.size() > 0);
  return result;
}

} // namespace svn::base