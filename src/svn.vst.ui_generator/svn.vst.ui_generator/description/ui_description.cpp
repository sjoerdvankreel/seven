#include <svn.base/topology/part_descriptor.hpp>
#include <svn.vst.ui_generator/support/support.hpp>
#include <svn.vst.ui_generator/description/support.hpp>
#include <svn.vst.ui_generator/description/ui_description.hpp>

#include <map>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace svn::vst::ui_generator {

static bool 
param_inside_graph(part_ui_description const& part, std::int32_t grid_index)
{
  if (part.graph == nullptr) return false;
  std::int32_t row = grid_index / part.columns;
  std::int32_t column = grid_index % part.columns;
  if(!(part.graph->row <= row && row < part.graph->row + part.graph->row_span)) return false;
  if(!(part.graph->column <= column && column < part.graph->column + part.graph->column_span)) return false;
  return true;
}

part_ui_description 
part_ui_description::create(
  svn::base::topology_info const& topology,
  std::int32_t runtime_part_index)
{
  part_ui_description result;
  auto const& part = topology.parts[runtime_part_index];
  result.graph = part.descriptor->graph;
  std::int32_t cell_count = 0;
  for(std::int32_t i = 0; i < part.descriptor->param_count; i++)
    cell_count = std::max(cell_count, part.descriptor->params[i].ui.param_index + 1);
  if (part.descriptor->ui.enabled_param!= -1) cell_count--;

  if(result.graph != nullptr) cell_count += result.graph->row_span * result.graph->column_span;
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

  // Map ui index to parameter list.
  std::map<std::int32_t, std::vector<int32_t>> layout;
  for (std::int32_t i = 0; i < part.descriptor->param_count; i++)
  {
    std::int32_t ui_index = part.descriptor->params[i].ui.param_index;
    if(part.descriptor->ui.enabled_param == i) 
    {
      result.enabled_param.runtime_param_index = part.runtime_param_start + i;
      continue;
    }
    if(layout.find(ui_index) == layout.end()) 
      layout.insert(std::make_pair(ui_index, std::vector<std::int32_t>()));
    layout.at(ui_index).push_back(i);
  }

  // Build param positions around the (optional) graph.
  int x;
  std::cin >> x;
  std::int32_t grid_index = 0;
  for(auto ui_index = layout.begin(); ui_index != layout.end(); ui_index++)
  {
    while (param_inside_graph(result, grid_index))
      grid_index++;
    for(auto param_index = ui_index->second.begin(); param_index != ui_index->second.end(); param_index++)
    {
      std::int32_t row = grid_index / result.columns;
      std::int32_t column = grid_index % result.columns;
      result.params.push_back({ row, column, part.runtime_param_start + *param_index });
    }
    grid_index++;
  }

  return result;
}

controller_ui_description
controller_ui_description::create(
  svn::base::topology_info const& topology)
{
  // Build part list.
  controller_ui_description result;
  for (std::int32_t p = 0; p < topology.static_part_count; p++)
  {
    auto const& static_part = topology.static_parts[p];
    for (std::int32_t c = 0; c < static_part.part_count; c++)
    {
      std::int32_t runtime_part_index = topology.part_bounds[p][c];
      part_ui_description description(part_ui_description::create(topology, runtime_part_index));
      if (description.height + 2 * margin > topology.ui.max_height)
        throw std::runtime_error("Part height exceeds max ui height.");
      result.parts.push_back(description);
    }
  }

  // Fix up top/left/column.
  std::int32_t column = 0;
  std::int32_t top = margin;
  std::int32_t left = margin;
  std::int32_t column_width = 0;
  std::int32_t max_column_height = 0;
  for (std::size_t p = 0; p < result.parts.size(); p++)
  {
    auto& part = result.parts[p];
    if (top + part.height + margin > topology.ui.max_height)
    {
      max_column_height = std::max(top, max_column_height);
      column++;
      top = margin;
      left += column_width + margin;
      result.column_widths.push_back(column_width);
      column_width = 0;
    }

    part.top = top;
    part.left = left;
    part.column = column;
    column_width = std::max(column_width, part.width);
    top += part.height + margin;
  }
  result.column_widths.push_back(column_width);
  max_column_height = std::max(top, max_column_height);

  // Fix up column width.
  result.width = margin;
  for (std::size_t c = 0; c < result.column_widths.size(); c++)
    result.width += result.column_widths[c] + margin;
  for (std::size_t p = 0; p < result.parts.size(); p++)
    result.parts[p].width = result.column_widths[result.parts[p].column];

  // Fix up last block height.
  result.height = max_column_height;
  for (std::size_t p = 0; p < result.parts.size(); p++)
    if (p == result.parts.size() - 1 || result.parts[p].column != result.parts[p + 1].column)
      result.parts[p].height = result.height - result.parts[p].top - margin;

  return result;
}

void 
controller_ui_description::print(svn::base::topology_info const& topology, std::ostream& os)
{
  os << "UI descriptor:\n";
  os << "\tWidth: " << width << "\n";
  os << "\tHeight: " << height << "\n";
  os << "\tColumn widths: ";
  for (std::size_t c = 0; c < column_widths.size(); c++)
    os << column_widths[c] << " ";
  os << "\n";
  for (std::size_t rp = 0; rp < parts.size(); rp++)
  {
    std::int32_t rt_index = parts[rp].runtime_part_index;
    os << "\tPart " << (rp + 1) << ":\n";
    os << "\t\tName: " << narrow_assume_ascii(topology.parts[rt_index].runtime_name) << "\n";
    os << "\t\tColumn: " << parts[rp].column << "\n";
    os << "\t\tParams: " << parts[rp].params.size() << "\n";
    os << "\t\tColumns: " << parts[rp].columns << "\n";
    os << "\t\tRows: " << parts[rp].rows << "\n";
    os << "\t\tWidth: " << parts[rp].width << "\n";
    os << "\t\tHeight: " << parts[rp].height << "\n";
    os << "\t\tLeft: " << parts[rp].left << "\n";
    os << "\t\tTop: " << parts[rp].top << "\n";
    os << "\t\tColor index: " << parts[rp].color_index << "\n";
    os << "\t\tEnabled index: " << parts[rp].enabled_param.runtime_param_index << "\n";
  }
  os << "\n";
}

} // namespace svn::vst::ui_generator