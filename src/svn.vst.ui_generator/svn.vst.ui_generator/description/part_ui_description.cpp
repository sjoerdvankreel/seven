#include <svn.base/topology/part_descriptor.hpp>
#include <svn.base/topology/param_descriptor.hpp>
#include <svn.vst.ui_generator/description/ui_description.hpp>
#include <svn.vst.ui_generator/description/part_ui_description.hpp>
#include <algorithm>

namespace svn::vst::ui_generator {

part_ui_description 
part_ui_description::create(
  svn::base::runtime_topology const& topology,
  std::int32_t runtime_part_index)
{
  part_ui_description result;
  auto const& part = topology.parts[runtime_part_index];
  std::int32_t cell_count = 0;
  for(std::int32_t i = 0; i < part.descriptor->param_count; i++)
    cell_count = std::max(cell_count, part.descriptor->params[i].ui.param_index + 1);
  if (part.descriptor->ui.enabled_param!= -1) cell_count--;

  result.runtime_part_index = runtime_part_index;
  result.columns = part.descriptor->ui.param_columns;
  result.rows = cell_count / result.columns;
  if (cell_count % result.columns != 0) ++result.rows;
  result.width = result.columns * param_total_width + margin;
  result.height = (result.rows + 1) * (param_row_height + margin);
  result.color_index = part.descriptor->ui.color_index % color_count;

  result.enabled_param.row = 0;
  result.enabled_param.runtime_param_index = -1;
  result.enabled_param.column = result.columns - 1;
  for (std::int32_t i = 0; i < part.descriptor->param_count; i++)
    if (i == part.descriptor->ui.enabled_param)
      result.enabled_param.runtime_param_index = part.runtime_param_start + i;
    else
    {
      std::int32_t grid_index = part.descriptor->params[i].ui.param_index;
      std::int32_t row = grid_index / result.columns;
      std::int32_t column = grid_index % result.columns;
      result.params.push_back({ row, column, part.runtime_param_start + i });
      grid_index++;
    } 

  return result;
}

} // namespace svn::vst::ui_generator