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
  for (std::int32_t p = 0; p < input_param_count; p++)
    switch (params[p].descriptor->type)
    {      
    case param_type::real: state[p].real = params[p].descriptor->default_.real; break;
    default: state[p].discrete = params[p].descriptor->default_.discrete; break;
    }   
}    
  
std::unique_ptr<runtime_topology>
runtime_topology::create(
  part_descriptor const* static_parts, std::int32_t part_count, 
  std::int32_t max_note_events, std::int32_t max_ui_height)
{
  bool seen_output = false;   
  assert(part_count > 0);
  assert(max_ui_height >= 0); 
  assert(max_note_events >= 0);
  assert(static_parts != nullptr);     

  auto result = std::make_unique<runtime_topology>();
  result->input_param_count = 0;
  result->output_param_count = 0; 
  result->static_parts = static_parts;
  result->static_part_count = part_count;
  result->max_ui_height = max_ui_height;
  result->max_note_events = max_note_events;

  std::int32_t part_index = 0; 
  std::int32_t param_index = 0;
  for (std::int32_t t = 0; t < part_count; t++)
  {             
    seen_output |= static_parts[t].output;
    assert(!seen_output || static_parts[t].output);
    std::int32_t type_index = 0;   
    result->part_bounds.push_back(std::vector<std::int32_t>());
    result->param_bounds.push_back(std::vector<std::int32_t>());
    std::wstring type_name(static_parts[t].static_name.short_);
    for (std::int32_t i = 0; i < static_parts[t].part_count; i++)
    {   
      result->part_bounds[t].push_back(part_index);
      result->param_bounds[t].push_back(param_index);
      std::wstring runtime_name = type_name;
      if(static_parts[t].output) result->output_param_count += static_parts[t].param_count;
      else result->input_param_count += static_parts[t].param_count;
      if (static_parts[t].part_count > 1) runtime_name += std::wstring(L" ") + std::to_wstring(i + 1);
      result->parts.push_back(runtime_part(type_index++, runtime_name, param_index, &static_parts[t]));
      part_index++;
      param_index += static_parts[t].param_count;
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

  result->ui_param_dependencies.resize(result->params.size());
  for (std::size_t p = 0; p < result->params.size(); p++)
  {
    auto const& this_param = result->params[p];
    if(this_param.descriptor->ui_relevant_if_param == -1) continue;
    std::int32_t index = this_param.part->type_index;
    std::int32_t type = this_param.part->descriptor->type;
    std::int32_t param = this_param.descriptor->ui_relevant_if_param;
    std::int32_t that_index = result->param_bounds[type][index] + param;
    result->ui_param_dependencies[p].push_back(that_index);
  }
   
  assert(result->parts.size() > 0);
  assert(result->params.size() > 0);
  assert(result->part_bounds.size() > 0);
  assert(result->param_bounds.size() > 0);
  assert(result->params.size() == result->input_param_count + result->output_param_count);
  return result;
} 

} // namespace svn::base