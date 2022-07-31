#include <svn.base/topology/topology_info.hpp>
#include <memory>
#include <cassert>

namespace svn::base {

void 
topology_info::init_defaults(param_value* state) const
{   
  for (std::int32_t p = 0; p < input_param_count; p++)
    switch (params[p].descriptor->type)
    {      
    case param_type::real: state[p].real = params[p].descriptor->real.default_; break;
    default: state[p].discrete = params[p].descriptor->discrete.default_; break;
    }   
}     

void
topology_info::state_check(param_value const* state) const
{
  for (std::int32_t p = 0; p < input_param_count; p++)
  {
    auto const& descriptor = *params[p].descriptor;
    if (descriptor.type == param_type::real)
      assert(0.0 <= state[p].real && state[p].real <= 1.0);
    else
    {
      std::int32_t val = state[p].discrete;
      assert(descriptor.discrete.min <= val && val <= descriptor.discrete.max);
    }
  }
}
  
topology_info const*
topology_info::create(
  part_descriptor const* static_parts, std::int32_t part_count, 
  std::int32_t max_note_events, std::int32_t max_ui_height)
{
  bool seen_output = false;   
  assert(part_count > 0);
  assert(max_ui_height >= 0); 
  assert(max_note_events >= 0);
  assert(static_parts != nullptr);     

  auto result = std::make_unique<topology_info>();
  result->input_param_count = 0;
  result->output_param_count = 0;
  result->static_parts = static_parts;
  result->static_part_count = part_count;
  result->max_note_events = max_note_events;
  result->ui.max_height = max_ui_height;

  std::int32_t part_index = 0; 
  std::int32_t param_index = 0;
  for (std::int32_t t = 0; t < part_count; t++)
  {             
    seen_output |= static_parts[t].output;
    assert(!seen_output || static_parts[t].output);
    std::int32_t type_index = 0;   
    result->part_bounds.push_back(std::vector<std::int32_t>());
    result->param_bounds.push_back(std::vector<std::int32_t>());
    std::string type_name(static_parts[t].static_name.short_);
    for (std::int32_t i = 0; i < static_parts[t].part_count; i++)
    {   
      result->part_bounds[t].push_back(part_index);
      result->param_bounds[t].push_back(param_index);
      std::string runtime_name = type_name;
      if(static_parts[t].output) result->output_param_count += static_parts[t].param_count;
      else result->input_param_count += static_parts[t].param_count;
      if (static_parts[t].part_count > 1) runtime_name += std::string(" ") + std::to_string(i + 1);
      result->parts.push_back(part_info({ type_index++, runtime_name, param_index, &static_parts[t] }));
      part_index++;
      param_index += static_parts[t].param_count;
    }
  }

  for (std::int32_t part = 0; part < result->parts.size(); part++)
  {
    std::string part_name = result->parts[part].runtime_name;
    for (std::int32_t p = 0; p < result->parts[part].descriptor->param_count; p++)
    {
      auto const& descriptor = result->parts[part].descriptor->params[p];
      std::string runtime_name = part_name + " " + descriptor.static_name.detail;
      result->params.push_back(param_info({ part, runtime_name, &descriptor }));
    }
  }

  result->ui.param_dependencies.resize(result->params.size());
  for (std::size_t p = 0; p < result->params.size(); p++)
  {
    auto const& this_param = result->params[p];
    auto const& this_descriptor = *this_param.descriptor;
    if(this_descriptor.ui.relevance_count == 0) continue;
    std::int32_t index = result->parts[this_param.part_index].type_index;
    std::int32_t type = result->parts[this_param.part_index].descriptor->type;
    for(std::int32_t i = 0; i < this_descriptor.ui.relevance_count; i++)
    {
      std::int32_t that_param = this_param.descriptor->ui.relevance[i].if_param;
      std::int32_t that_index = result->param_bounds[type][index] + that_param;
      result->ui.param_dependencies[that_index].push_back(p);
    }
  }
   
  assert(result->parts.size() > 0);
  assert(result->params.size() > 0);
  assert(result->part_bounds.size() > 0);
  assert(result->param_bounds.size() > 0);
  assert(result->params.size() == result->input_param_count + result->output_param_count);
  return result.release();
} 

} // namespace svn::base