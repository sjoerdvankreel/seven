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
  result.occupied_cell_count = 0;
  auto const& part = topology.parts[runtime_part_index];
  result.graphs = part.descriptor->graphs;
  result.graph_count = part.descriptor->graph_count;

  for(std::int32_t i = 0; i < part.descriptor->param_count; i++)
    result.occupied_cell_count = std::max(result.occupied_cell_count, part.descriptor->params[i].ui.param_index + 1);
  for(std::int32_t g = 0; g < result.graph_count; g++)
    result.occupied_cell_count += result.graphs[g].row_span * result.graphs[g].column_span;

  result.runtime_part_index = runtime_part_index;
  result.columns = part.descriptor->ui.param_columns;
  result.rows = result.occupied_cell_count / result.columns;
  if (result.occupied_cell_count % result.columns != 0) ++result.rows;
  result.width = result.columns * param_total_width;
  result.height = (result.rows + 1) * (param_row_height + margin) + padding_param_group * 4;

  result.enabled_param.row = 0;
  result.enabled_param.runtime_param_index = -1;
  result.enabled_param.column = result.columns - 1;

  // Map ui index to parameter list (param id and skip-before count).
  std::map<std::int32_t, std::vector<std::int32_t>> layout;
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
      layout.insert(std::make_pair(ui_index, std::vector<std::int32_t>()));
    layout.at(ui_index).push_back(i);
  }
   
  // Build param positions around the (optional) graphs.
  std::int32_t grid_index = 0;
  for(auto ui_index = layout.begin(); ui_index != layout.end(); ui_index++)
  {
    while (param_inside_graphs(result, grid_index))
      grid_index++;
    for (auto param_index = ui_index->second.begin(); param_index != ui_index->second.end(); param_index++)
    {
      std::int32_t row = grid_index / result.columns;
      std::int32_t column = grid_index % result.columns;
      result.params.push_back({ row, column, part.runtime_param_start + *param_index });
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

part_type_ui_description
part_type_ui_description::create(
  svn::base::topology_info const& topology, 
  svn::base::part_descriptor const& descriptor,
  std::int32_t runtime_selector_indices_start,
  std::vector<std::int32_t> runtime_part_indices)
{
  part_type_ui_description result;
  result.info = descriptor.ui.info;
  result.selector_param.row = -1;
  result.selector_param.column = -1;
  result.selector_param.runtime_param_index = -1;
  result.name = descriptor.static_name.short_;
  result.color = ui_color_gradient(topology, descriptor.ui.part_index);
  for(std::int32_t i = 0; i < descriptor.part_count; i++)
    result.parts.push_back(part_ui_description::create(topology, runtime_part_indices[i]));
  if (descriptor.ui.selector_param != -1)
    result.selector_param.runtime_param_index = runtime_selector_indices_start + descriptor.ui.selector_param;
  result.width = result.parts[0].width;
  result.height = result.parts[0].height;
  return result;
}

controller_ui_description
controller_ui_description::create(
  svn::base::topology_info const& topology)
{
  // Build part list.
  std::int32_t selector_part = -1;
  controller_ui_description result;
  std::vector<std::pair<std::int32_t, std::int32_t>> part_type_ui_indices;
  for(std::int32_t i = 0; i < topology.static_part_count; i++)
    if(topology.static_parts[i].selector)
      selector_part = i;
    else
      part_type_ui_indices.push_back(std::make_pair(i, topology.static_parts[i].ui.part_index));

  std::int32_t runtime_selector_indices_start = -1;
  if(selector_part != -1) runtime_selector_indices_start = topology.param_bounds[selector_part][0];
  std::sort(part_type_ui_indices.begin(), part_type_ui_indices.end(), 
    [](auto const& l, auto const& r) -> bool { return l.second < r.second; });
  for (std::size_t p = 0; p < part_type_ui_indices.size(); p++)
  {
    std::int32_t index = part_type_ui_indices[p].first;
    auto const& static_part = topology.static_parts[index];
    result.part_types.push_back(part_type_ui_description::create(
      topology, static_part, runtime_selector_indices_start, topology.part_bounds[index]));
  }

  // Fix up top/left/column.
  std::int32_t column = 0;
  std::int32_t top = margin;
  std::int32_t left = margin;
  std::int32_t column_width = 0;
  std::int32_t max_column_height = 0;
  for (std::size_t p = 0; p < result.part_types.size(); p++)
  {
    auto& part_type = result.part_types[p];
    if (top + part_type.height + margin > topology.ui.max_height)
    {
      max_column_height = std::max(top, max_column_height);
      column++;
      top = margin;
      left += column_width + margin;
      result.column_widths.push_back(column_width);
      column_width = 0;
    }

    part_type.top = top;
    part_type.left = left;
    part_type.column = column;
    column_width = std::max(column_width, part_type.width);
    top += part_type.height + margin;
  }
  result.column_widths.push_back(column_width);
  max_column_height = std::max(top, max_column_height);

  // Fix up column width.
  result.width = margin;
  for (std::size_t c = 0; c < result.column_widths.size(); c++)
    result.width += result.column_widths[c] + margin;
  for (std::size_t pt = 0; pt < result.part_types.size(); pt++)
  {
    result.part_types[pt].width = result.column_widths[result.part_types[pt].column];
    for (std::size_t p = 0; p < result.part_types[pt].parts.size(); p++)
      result.part_types[pt].parts[p].width = result.part_types[pt].width;
  }

  // Fix up last block height.
  result.height = max_column_height;
  for (std::size_t pt = 0; pt < result.part_types.size(); pt++)
    if (pt == result.part_types.size() - 1 || result.part_types[pt].column != result.part_types[pt + 1].column)
    {
      result.part_types[pt].height = result.height - result.part_types[pt].top - margin;
      for(std::size_t p = 0; p < result.part_types[pt].parts.size(); p++)
        result.part_types[pt].parts[p].height = result.part_types[pt].height;
    }

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
  for (std::size_t part_type = 0; part_type < part_types.size(); part_type++)
  {
    os << "\tPart type " << part_type << ":\n";
    os << "\t\tName: " << narrow_assume_ascii(part_types[part_type].name) << "\n";
    os << "\t\tTop: " << part_types[part_type].top << "\n";
    os << "\t\tLeft: " << part_types[part_type].left << "\n";
    os << "\t\tWidth: " << part_types[part_type].width << "\n";
    os << "\t\tHeight: " << part_types[part_type].height << "\n";
    os << "\t\tColumn: " << part_types[part_type].column << "\n";
    os << "\t\tSelector: " << part_types[part_type].selector_param.runtime_param_index << "\n";
    os << "\t\tInfo: " << narrow_assume_ascii(part_types[part_type].info) << "\n";
    os << "\t\tColor:";
    os << " " << static_cast<std::int32_t>(part_types[part_type].color.r);
    os << " " << static_cast<std::int32_t>(part_types[part_type].color.g);
    os << " " << static_cast<std::int32_t>(part_types[part_type].color.b);
    os << "\n";

    for(std::size_t part = 0; part < part_types[part_type].parts.size(); part++)
    {
      auto const& part_desc = part_types[part_type].parts[part];
      std::int32_t rt_index = part_desc.runtime_part_index;
      os << "\t\tPart " << part << ":\n";
      os << "\t\t\tName: " << narrow_assume_ascii(topology.parts[rt_index].runtime_name) << "\n";
      os << "\t\t\tParams: " << part_desc.params.size() << "\n";
      os << "\t\t\tColumns: " << part_desc.columns << "\n";
      os << "\t\t\tRows: " << part_desc.rows << "\n";
      os << "\t\t\tWidth: " << part_desc.width << "\n";
      os << "\t\t\tHeight: " << part_desc.height << "\n";
      os << "\t\t\tOccupied cell count: " << part_desc.occupied_cell_count << "\n";
      os << "\t\t\tEnabled index: " << part_desc.enabled_param.runtime_param_index << "\n";
  
      for(std::int32_t g = 0; g < part_desc.graph_count; g++)
      {
        os << "\t\t\tGraph " << (g + 1) << " position: ";
        os << "top: " << part_desc.graphs[g].row << ", ";
        os << "left: " << part_desc.graphs[g].column << ", ";
        os << "height: " << part_desc.graphs[g].row_span << ", ";
        os << "width: " << part_desc.graphs[g].column_span << "\n";
      }
    
      os << "\t\t\tParam cells:\n";
      for (std::size_t param = 0; param < part_desc.params.size(); param++)
      {
        auto const& param_ui = part_desc.params[param];
        std::string name = "(Filler)";
        if(param_ui.runtime_param_index != -1)
        {
          auto const& descriptor = *topology.params[param_ui.runtime_param_index].descriptor;
          name = narrow_assume_ascii(descriptor.static_name.short_);
        }
        std::cout << "\t\t\t\t" << name << " (" << param_ui.row << ", " << param_ui.column << ")\n";
      }
    }
  }
  os << "\n";
}

} // namespace svn::vst::ui