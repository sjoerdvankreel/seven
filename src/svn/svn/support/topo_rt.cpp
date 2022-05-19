#include <svn/support/topo_rt.hpp>
#include <svn/support/topo_static.hpp>
#include <vector>

namespace svn {

std::int32_t synth_part_count;
synth_part const* synth_parts;
std::int32_t const* synth_part_bounds;

std::int32_t synth_param_count;
synth_param const* synth_params;
std::int32_t const* synth_param_bounds;

namespace {

struct init_t { init_t(); };
static init_t init_;

static std::vector<synth_part> synth_parts_;
static std::vector<synth_param> synth_params_;

static std::vector<std::int32_t> synth_part_bounds_;
static std::vector<std::int32_t> synth_param_bounds_;

init_t::
init_t()
{
  std::int32_t prev_type = -1;
  std::int32_t part_index = 0;
  for(std::int32_t t = 0; t < part_type::count; t++)
  {
    if(t != prev_type)
      synth_part_bounds_.push_back(part_index);
    for(std::int32_t i = 0; i < part_infos[t].count; i++)
      synth_parts_.push_back({ part_index++, &part_infos[t] });
  }
  synth_parts = synth_parts_.data();
  synth_part_bounds = synth_part_bounds_.data();
  synth_part_count = static_cast<std::int32_t>(synth_parts_.size());

  std::int32_t param_index = 0;
  for(std::int32_t sp = 0; sp < synth_part_count; sp++)
  {
    synth_param_bounds_.push_back(param_index);
    for(std::int32_t p = 0; p < synth_parts[sp].info->param_count; p++, param_index++)
      synth_params_.push_back({ &synth_parts[sp], &synth_parts[sp].info->params[p] });
  }
  synth_params = synth_params_.data();
  synth_param_bounds = synth_param_bounds_.data();
  synth_param_count = static_cast<std::int32_t>(synth_params_.size());
}

} // namespace anonymous
} // namespace svn