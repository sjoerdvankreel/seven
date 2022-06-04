#ifndef SVN_BASE_DSP_AUTOMATION_VIEW_HPP
#define SVN_BASE_DSP_AUTOMATION_VIEW_HPP

#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::base {

class automation_view
{
  void* const* _automation;
  std::int32_t _total_param_count;
  std::int32_t _part_param_count;
  std::int32_t _part_param_offset;
  std::int32_t _sample_count;
  std::int32_t _sample_offset;
  std::int32_t _sample_fixed_at;

public:
  automation_view(
    void* const* automation,
    std::int32_t total_param_count,
    std::int32_t part_param_count,
    std::int32_t part_param_offset,
    std::int32_t sample_count,
    std::int32_t sample_offset,
    std::int32_t sample_fixed_at);

  automation_view rearrange_samples(
    std::int32_t sample_offset,
    std::int32_t sample_fixed_at) const;
  automation_view rearrange_params(
    std::int32_t part_param_count,
    std::int32_t part_param_offset) const;

  template <typename T> T
  get(std::int32_t param, std::int32_t sample) const;
};

inline automation_view::
automation_view(
  void* const* automation,
  std::int32_t total_param_count,
  std::int32_t part_param_count,
  std::int32_t part_param_offset,
  std::int32_t sample_count,
  std::int32_t sample_offset,
  std::int32_t sample_fixed_at) :
_automation(automation),
_total_param_count(total_param_count),
_part_param_count(part_param_count),
_part_param_offset(part_param_offset),
_sample_count(sample_count),
_sample_offset(sample_offset),
_sample_fixed_at(sample_fixed_at)
{
  assert(automation != nullptr);
  assert(part_param_count > 0);
  assert(total_param_count > 0);
  assert(part_param_offset > 0);
  assert(part_param_offset < total_param_count);
  assert(total_param_count >= part_param_count);
  assert(sample_count > 0);
  assert(sample_offset >= 0);
  assert(sample_offset < sample_count);
  assert(sample_fixed_at <= sample_count);
  assert(sample_fixed_at >= sample_offset);
}

inline automation_view 
automation_view::rearrange_samples(
  std::int32_t sample_offset,
  std::int32_t sample_fixed_at) const
{ 
  return automation_view(_automation, 
   _total_param_count, _part_param_count, _part_param_offset, 
   _sample_count, sample_offset, sample_fixed_at); 
}

inline automation_view
automation_view::rearrange_params(
  std::int32_t part_param_count,
  std::int32_t part_param_offset) const
{
  return automation_view(_automation,
    _total_param_count, part_param_count, part_param_offset,
    _sample_count, _sample_offset, _sample_fixed_at);
}

template <typename T>
inline T 
automation_view::get(std::int32_t param, std::int32_t sample) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(sample >= 0);
  assert(sample < _sample_count - _sample_offset);
  auto buffer = static_cast<T*>(_automation[_part_param_offset + param]);
  return buffer[std::min(sample + _sample_offset, _sample_fixed_at)];
}

} // namespace svn::base
#endif // SVN_BASE_DSP_AUTOMATION_VIEW_HPP