#include <svn.base/static/part_descriptor.hpp>
#include <cassert>

namespace svn::base {

part_descriptor::
part_descriptor(
  std::int32_t type, bool output, item_name const& static_name, 
  std::int32_t part_count, param_descriptor const* params, 
  std::int32_t param_count, std::int32_t ui_control_columns, 
  std::int32_t ui_control_enabled, std::int32_t ui_color_index) :
type(type), output(output), static_name(static_name), 
part_count(part_count), param_count(param_count), 
params(params), ui_control_columns(ui_control_columns), 
ui_control_enabled(ui_control_enabled), ui_color_index(ui_color_index)
{  
  assert(part_count > 0);
  assert(param_count > 0);
  assert(params != nullptr); 
  assert(ui_color_index >= 0);
  assert(ui_control_columns >= 0);
  assert(ui_control_enabled >= -1 && ui_control_enabled < param_count);
} 

} // namespace svn::base