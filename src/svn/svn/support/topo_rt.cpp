#include <svn/support/topo_rt.hpp>
#include <svn/support/param_value.hpp>
#include <svn/support/topo_static.hpp>
#include <vector>

namespace svn {

std::int32_t synth_part_count;
std::int32_t synth_param_count;

synth_part const* synth_parts = nullptr;
synth_param const* synth_params = nullptr;
std::int32_t const* const* synth_bounds = nullptr;

std::vector<synth_part> synth_parts_;
std::vector<synth_param> synth_params_;
std::vector<std::int32_t const*> synth_bounds_;
std::vector<std::vector<std::int32_t>> param_bounds_;

void
destroy_topology()
{
  synth_parts_.clear();
  synth_params_.clear();
  synth_bounds_.clear();
  param_bounds_.clear();
  synth_parts = nullptr;
  synth_params = nullptr;
  synth_bounds = nullptr;
}

void
init_defaults(param_value* state)
{
  for (std::int32_t p = 0; p < synth_param_count; p++)
    switch (synth_params[p].info->type)
    {
    case param_type::real: state[p].real = synth_params[p].info->default_.real; break;
    default: state[p].discrete = synth_params[p].info->default_.discrete; break;
    }
}

void 
init_topology()
{
  std::int32_t param_index = 0;
  for (std::int32_t t = 0; t < part_type::count; t++)
  {
    param_bounds_.push_back(std::vector<std::int32_t>());
    for (std::int32_t i = 0; i < part_infos[t].count; i++)
    {
      param_bounds_[t].push_back(param_index);
      param_index += part_infos[t].param_count;
    }
    synth_bounds_.push_back(param_bounds_[t].data());
  }
  synth_bounds = synth_bounds_.data();

  for(std::int32_t t = 0; t < part_type::count; t++)
  {
    std::int32_t part_index = 0;
    std::wstring name(part_infos[t].item.name);
    for(std::int32_t i = 0; i < part_infos[t].count; i++)
    {
      std::wstring part_name = name;
      if(part_infos[t].count > 1) part_name += std::wstring(L" ") + std::to_wstring(i + 1);
      synth_parts_.push_back({ part_name, part_index++, &part_infos[t]});
    }
  }

  synth_parts = synth_parts_.data();
  synth_part_count = static_cast<std::int32_t>(synth_parts_.size());

  for(std::int32_t sp = 0; sp < synth_part_count; sp++)
    for(std::int32_t p = 0; p < synth_parts[sp].info->param_count; p++)
      synth_params_.push_back({ &synth_parts[sp], sp, &synth_parts[sp].info->params[p] });
  synth_params = synth_params_.data();
  synth_param_count = static_cast<std::int32_t>(synth_params_.size());
}

} // namespace svn