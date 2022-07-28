#ifndef SVN_BASE_DSP_AUTOMATION_VIEW_HPP
#define SVN_BASE_DSP_AUTOMATION_VIEW_HPP

#include <svn.base/topology/topology_info.hpp>

#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::base {

// View into automation buffer.
// TODO make everything array based.
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
  
  // Automation stuff takes care of _fixed.
  float automation_real(std::int32_t param, std::int32_t sample) const;
  param_value automation_param(std::int32_t param, std::int32_t sample) const;
  std::int32_t automation_discrete(std::int32_t param, std::int32_t sample) const;
  void automation_real(std::int32_t param, float* cv_out, std::int32_t count) const;

  // Transform stuff assumes _fixed has already been taken care of.
  void transform_real(std::int32_t param, float* cv_inout, std::int32_t count) const;

  // Rearrange part or sample indices.
  automation_view rearrange_params(std::int32_t part_type, std::int32_t part_index) const;
  automation_view rearrange_samples(std::int32_t sample_offset, std::int32_t sample_fixed_at) const;

  // Clean up later.
  float to_dsp(std::int32_t param, float val) const;
  float from_dsp(std::int32_t param, float val) const;
  float get_real_dsp(std::int32_t param, std::int32_t sample) const;
  float get_transformed_dsp(std::int32_t param, std::int32_t sample, float const* const* transformed_cv) const;

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
automation_view::automation_param(std::int32_t param, std::int32_t sample) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(sample >= 0);
  assert(sample < _sample_count - _sample_offset);
  if (sample >= _sample_fixed_at) return _fixed[_part_param_offset + param];
  return _automation[param + _part_param_offset][sample + _sample_offset];
}

inline float
automation_view::automation_real(std::int32_t param, std::int32_t sample) const
{ 
  assert(_topology->params[param + _part_param_offset].descriptor->type == param_type::real); 
  return automation_param(param, sample).real;
}

// TODO - always get at sample 0
inline std::int32_t
automation_view::automation_discrete(std::int32_t param, std::int32_t sample) const
{ 
  assert(_topology->params[param + _part_param_offset].descriptor->type != param_type::real);
  return automation_param(param, sample).discrete;
}

inline void
automation_view::automation_real(std::int32_t param, float* cv_out, std::int32_t count) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(count >= 0);
  assert(count <= _sample_count - _sample_offset);
  assert(_topology->params[param + _part_param_offset].descriptor->type == param_type::real);
  std::int32_t automated_count = std::min(count, _sample_fixed_at);
  // Better hope punning works everywhere.
  float const* automated = reinterpret_cast<float const*>(_automation[param + _part_param_offset]);
  std::copy(automated, automated + automated_count, cv_out);
  std::int32_t fixed_count = std::max(0, count - _sample_fixed_at);
  assert(fixed_count == 0 || _fixed != nullptr);
  if (fixed_count > 0) std::fill(cv_out + automated_count, cv_out + automated_count + fixed_count, _fixed[_part_param_offset + param].real);
}

inline void 
automation_view::transform_real(std::int32_t param, float* cv_inout, std::int32_t count) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(count >= 0);
  assert(count <= _sample_count - _sample_offset);
  assert(_topology->params[param + _part_param_offset].descriptor->type == param_type::real);
  _topology->params[param + _part_param_offset].descriptor->real.dsp.to_range(cv_inout, count);
}

inline float
automation_view::to_dsp(std::int32_t param, float val) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  return _topology->params[param + _part_param_offset].descriptor->real.dsp.to_range(val);
}

inline float
automation_view::from_dsp(std::int32_t param, float val) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  return _topology->params[param + _part_param_offset].descriptor->real.dsp.from_range(val);
}

// TODO REMOVE - does both _fixed and then returns output
inline float
automation_view::get_real_dsp(std::int32_t param, std::int32_t sample) const
{ return to_dsp(param, automation_real(param, sample)); }

// TODO REMOVE
inline float 
automation_view::get_transformed_dsp(std::int32_t param, std::int32_t sample, float const* const* transformed_cv) const
{ 
  assert(param >= 0);
  assert(sample >= 0);
  assert(param < _part_param_count);
  assert(sample < _sample_count - _sample_offset);
  return transformed_cv[param][sample];
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