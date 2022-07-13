#include <svn.base/topology/part_descriptor.hpp>
#include <svn.vst.ui/support/support.hpp>
#include <svn.vst.ui/description/support.hpp>
#include <svn.vst.ui/description/ui_description.hpp>

#include <map>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace svn::vst::ui {

static bool 
param_inside_graph(part_ui_description const& part, std::int32_t graph_index, std::int32_t grid_index)
{
  auto const& graph = part.graphs[graph_index];
  std::int32_t row = grid_index / part.columns;
  std::int32_t column = grid_index % part.columns;
  if(!(graph.row <= row && row < graph.row + graph.row_span)) return false;
  if(!(graph.column <= column && column < graph.column + graph.column_span)) return false;
  return true;
}

static bool 
param_inside_graphs(part_ui_description const& part, std::int32_t grid_index)
{
  for(std::int32_t g = 0; g < part.graph_count; g++)
    if(param_inside_graph(part, g, grid_index)) return true;
  return false;
}

part_ui_description 
part_ui_description::create(
  svn::base::topology_info const& topology,
  std::int32_t runtime_part_index)
{
  part_ui_description result;
  std::int32_t skip_cells = 0;
  result.occupied_cell_count = 0;
  auto const& part = topology.parts[runtime_part_index];
  result.info = part.descriptor->ui.info;
  result.graphs = part.descriptor->graphs;
  result.graph_count = part.descriptor->graph_count;

  for(std::int32_t i = 0; i < part.descriptor->param_count; i++)
  {
    skip_cells += part.descriptor->params[i].ui.skip_before;
    result.occupied_cell_count = std::max(result.occupied_cell_count, part.descriptor->params[i].ui.param_index + 1);
  }
  result.occupied_cell_count += skip_cells;
  for(std::int32_t g = 0; g < result.graph_count; g++)
    result.occupied_cell_count += result.graphs[g].row_span * result.graphs[g].column_span;

  result.runtime_part_index = runtime_part_index;
  result.columns = part.descriptor->ui.param_columns;
  result.rows = result.occupied_cell_count / result.columns;
  if (result.occupied_cell_count % result.columns != 0) ++result.rows;
  result.color = part.descriptor->ui.color;
  result.width = result.columns * param_total_width;
  result.height = (result.rows + 1) * (param_row_height + margin);

  result.enabled_param.row = 0;
  result.enabled_param.runtime_param_index = -1;
  result.enabled_param.column = result.columns - 1;

  // Map ui index to parameter list (param id and skip-before count).
  std::map<std::int32_t, std::vector<std::pair<std::int32_t, std::int32_t>>> layout;
  for (std::int32_t i = 0; i < part.descriptor->param_count; i++)
  {
    auto const& ui_desc = part.descriptor->params[i].ui;
    std::int32_t ui_index = ui_desc.param_index;
    if (part.descriptor->ui.enabled_param == i)
    {
      result.enabled_param.runtime_param_index = part.runtime_param_start + i;
      continue;
    }
    if (layout.find(ui_index) == layout.end())
      layout.insert(std::make_pair(ui_index, std::vector<std::pair<std::int32_t, std::int32_t>>()));
    layout.at(ui_index).push_back(std::make_pair(i, ui_desc.skip_before));
    assert(ui_desc.skip_before == layout.at(ui_index).at(0).second);
  }
   
  // Build param positions around the (optional) graphs.
  std::int32_t grid_index = 0;
  for(auto ui_index = layout.begin(); ui_index != layout.end(); ui_index++)
  {
    while (param_inside_graphs(result, grid_index))
      grid_index++;
    grid_index += ui_index->second.at(0).second;
    while (param_inside_graphs(result, grid_index))
      grid_index++;
    for (auto param_index = ui_index->second.begin(); param_index != ui_index->second.end(); param_index++)
    {
      std::int32_t row = grid_index / result.columns;
      std::int32_t column = grid_index % result.columns;
      result.params.push_back({ row, column, part.runtime_param_start + param_index->first });
    }
    grid_index++;
  }

  // Add filler cells.
  for (; grid_index < result.rows * result.columns; grid_index++)
  {
    while (param_inside_graphs(result, grid_index))
      grid_index++;
    if(grid_index >= result.rows * result.columns) break;
    std::int32_t row = grid_index / result.columns;
    std::int32_t column = grid_index % result.columns;
    result.params.push_back({ row, column, -1 });
  }

  return result;
}

controller_ui_description
controller_ui_description::create(
  svn::base::topology_info const& topology)
{
  // Build part list.
  controller_ui_description result;
  std::vector<std::pair<std::int32_t, std::int32_t>> part_ui_indices;
  for(std::int32_t i = 0; i < topology.static_part_count; i++)
    part_ui_indices.push_back(std::make_pair(i, topology.static_parts[i].ui.part_index));
  std::sort(part_ui_indices.begin(), part_ui_indices.end(), [](auto const& l, auto const& r) -> bool { return l.second < r.second; });
  for (std::int32_t p = 0; p < topology.static_part_count; p++)
  {
    auto const& static_part = topology.static_parts[part_ui_indices[p].first];
    for (std::int32_t c = 0; c < static_part.part_count; c++)
    {
      std::int32_t runtime_part_index = topology.part_bounds[part_ui_indices[p].first][c];
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
  for (std::size_t part = 0; part < parts.size(); part++)
  {
    std::int32_t rt_index = parts[part].runtime_part_index;
    os << "\tPart " << part << ":\n";
    os << "\t\tName: " << narrow_assume_ascii(topology.parts[rt_index].runtime_name) << "\n";
    os << "\t\tColumn: " << parts[part].column << "\n";
    os << "\t\tParams: " << parts[part].params.size() << "\n";
    os << "\t\tColumns: " << parts[part].columns << "\n";
    os << "\t\tRows: " << parts[part].rows << "\n";
    os << "\t\tWidth: " << parts[part].width << "\n";
    os << "\t\tHeight: " << parts[part].height << "\n";
    os << "\t\tLeft: " << parts[part].left << "\n";
    os << "\t\tTop: " << parts[part].top << "\n";
    os << "\t\tOccupied cell count: " << parts[part].occupied_cell_count << "\n";
    os << "\t\tEnabled index: " << parts[part].enabled_param.runtime_param_index << "\n";
    os << "\t\tColor:";
    os << " " << static_cast<std::int32_t>(parts[part].color.r);
    os << " " << static_cast<std::int32_t>(parts[part].color.g);
    os << " " << static_cast<std::int32_t>(parts[part].color.b);
    os << "\n";
    for(std::int32_t g = 0; g < parts[part].graph_count; g++)
    {
      os << "\t\tGraph " << (g + 1) << " position: ";
      os << "top: " << parts[part].graphs[g].row << ", ";
      os << "left: " << parts[part].graphs[g].column << ", ";
      os << "height: " << parts[part].graphs[g].row_span << ", ";
      os << "width: " << parts[part].graphs[g].column_span << "\n";
    }
    os << "\t\tParam cells:\n";
    for (std::size_t param = 0; param < parts[part].params.size(); param++)
    {
      auto const& param_ui = parts[part].params[param];
      std::string name = "(Filler)";
      if(param_ui.runtime_param_index != -1)
      {
        auto const& descriptor = *topology.params[param_ui.runtime_param_index].descriptor;
        name = narrow_assume_ascii(descriptor.static_name.short_);
      }
      std::cout << "\t\t\t" << name << " (" << param_ui.row << ", " << param_ui.column << ")\n";
    }
  }
  os << "\n";
}

} // namespace svn::vst::ui