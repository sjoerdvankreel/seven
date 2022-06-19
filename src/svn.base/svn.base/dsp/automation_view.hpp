#ifndef SVN_BASE_DSP_AUTOMATION_VIEW_HPP
#define SVN_BASE_DSP_AUTOMATION_VIEW_HPP

#include <svn.base/topology/topology_info.hpp>

#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::base {

// View into automation buffer.
class automation_view
{
  std::int32_t _sample_count = 0;
  std::int32_t _sample_offset = 0;
  std::int32_t _sample_fixed_at = 0;

  std::int32_t _total_param_count = 0;
  std::int32_t _part_param_count = 0;
  std::int32_t _part_param_offset = 0;

  param_value const* _fixed = nullptr;
  topology_info const* _topology = nullptr;
  param_value const* const* _automation = nullptr;

public:
  automation_view() = default;
  param_value get(std::int32_t param, std::int32_t sample) const;
  automation_view rearrange_params(std::int32_t part_type, std::int32_t part_index) const;
  automation_view rearrange_samples(std::int32_t sample_offset, std::int32_t sample_fixed_at) const;

  automation_view(
    topology_info const* topology, param_value const* fixed, param_value const* const* automation,
    std::int32_t total_param_count, std::int32_t part_param_count, std::int32_t part_param_offset,
    std::int32_t sample_count, std::int32_t sample_offset, std::int32_t sample_fixed_at);
};

inline automation_view::
automation_view(
  topology_info const* topology, param_value const* fixed, param_value const* const* automation,
  std::int32_t total_param_count, std::int32_t part_param_count, std::int32_t part_param_offset,
  std::int32_t sample_count, std::int32_t sample_offset, std::int32_t sample_fixed_at):
_topology(topology), _fixed(fixed), _automation(automation),
_total_param_count(total_param_count), _part_param_count(part_param_count), _part_param_offset(part_param_offset),
_sample_count(sample_count), _sample_offset(sample_offset), _sample_fixed_at(sample_fixed_at)
{
  assert(topology != nullptr);
  assert(automation != nullptr);
  assert(part_param_count > 0);
  assert(total_param_count > 0);
  assert(part_param_offset >= 0);
  assert(part_param_offset < total_param_count);
  assert(total_param_count >= part_param_count);
  assert(sample_count > 0);
  assert(sample_offset >= 0);
  assert(sample_offset < sample_count);
  assert(sample_fixed_at >= 0);
  assert(sample_fixed_at <= sample_count - sample_offset);
  assert(sample_fixed_at == sample_count - sample_offset || fixed != nullptr);
}

inline param_value
automation_view::get(std::int32_t param, std::int32_t sample) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(sample >= 0);
  assert(sample < _sample_count - _sample_offset);
  if (sample >= _sample_fixed_at) return _fixed[_part_param_offset + param];
  return _automation[param + _part_param_offset][sample + _sample_offset];
}

inline automation_view
automation_view::rearrange_params(std::int32_t part_type, std::int32_t part_index) const
{
  std::int32_t param_count = _topology->static_parts[part_type].param_count;
  std::int32_t param_offset = _topology->param_bounds[part_type][part_index];
  return automation_view(
    _topology, _fixed, _automation,
    _total_param_count, param_count, param_offset,
    _sample_count, _sample_offset, _sample_fixed_at);
}

inline automation_view
automation_view::rearrange_samples(std::int32_t sample_offset, std::int32_t sample_fixed_at) const
{
  return automation_view(
    _topology, _fixed, _automation,
    _total_param_count, _part_param_count, _part_param_offset,
    _sample_count, sample_offset, sample_fixed_at);
}

} // namespace svn::base
#endif // SVN_BASE_DSP_AUTOMATION_VIEW_HPP