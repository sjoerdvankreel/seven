#include <svn/automation/unit_automation_buffer.hpp>

namespace svn {

template <typename sample_type>
void unit_automation_buffer<sample_type>::
setup(
  param_id first,
  param_id last,
  std::int32_t unit_index,
  automation_buffer<sample_type> const& automation)
{
  param_count = 0;
  std::int32_t offset = unit_index * (last - first + 1);
  for(std::int32_t i = 0; i < automation.param_count; i++)
  {
    param_automation_buffer<sample_type> const& param = automation.params[i];
    if (first + offset <= param.id && param.id <= last + offset)
    {
      params[param_count] = param.id;
      samples[param_count] = param.samples;
      ++param_count;
    }
  }
}

template class unit_automation_buffer<float>;
template class unit_automation_buffer<double>;

} // namespace svn