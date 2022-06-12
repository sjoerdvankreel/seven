#include <svn.base/static/part_descriptor.hpp>
#include <svn.vst.ui_generator/description/ui_description.hpp>
#include <svn.vst.ui_generator/description/controller_ui_description.hpp>

#include <stdexcept>
#include <algorithm>

namespace svn::vst::ui_generator {

controller_ui_description
controller_ui_description::create(
  svn::base::runtime_topology const& topology)
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
      if (description.height + 2 * margin > topology.max_ui_height)
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
    if (top + part.height + margin > topology.max_ui_height)
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
controller_ui_description::print(svn::base::runtime_topology const& topology, std::ostream& os)
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
    os << "\t\tName: " << topology.parts[rt_index].runtime_name << L"\n";
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