#include <svn.base/static/part_descriptor.hpp>
#include <svn.vst.ui_generator/description/ui_description.hpp>
#include <svn.vst.ui_generator/description/part_ui_description.hpp>

namespace svn::vst::ui_generator {

part_ui_description 
part_ui_description::create(
  svn::base::runtime_topology const& topology,
  std::int32_t runtime_part_index)
{
  part_ui_description result;
  auto const& part = topology.parts[runtime_part_index];
  std::int32_t param_count = part.descriptor->param_count;
  if (part.descriptor->ui_control_enabled != -1) param_count--;

  result.runtime_part_index = runtime_part_index;
  result.columns = part.descriptor->ui_control_columns;
  result.rows = param_count / result.columns;
  if (param_count % result.columns != 0) ++result.rows;
  result.width = result.columns * param_total_width + margin;
  result.height = (result.rows + 1) * (param_row_height + margin);
  result.color_index = part.descriptor->ui_color_index % color_count;

  std::int32_t grid_index = 0;
  result.enabled_param.row = 0;
  result.enabled_param.runtime_param_index = -1;
  result.enabled_param.column = result.columns - 1;
  for (std::int32_t i = 0; i < part.descriptor->param_count; i++)
    if (i == part.descriptor->ui_control_enabled)
      result.enabled_param.runtime_param_index = part.runtime_param_start + i;
    else
    {
      std::int32_t row = grid_index / result.columns;
      std::int32_t column = grid_index % result.columns;
      result.params.push_back({ row, column, part.runtime_param_start + i });
      grid_index++;
    }

  return result;
}

} // namespace svn::vst::ui_generator