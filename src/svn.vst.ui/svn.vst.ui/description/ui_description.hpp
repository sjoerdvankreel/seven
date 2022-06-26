#ifndef SVN_VST_UI_DESCRIPTION_UI_DESCRIPTION_HPP
#define SVN_VST_UI_DESCRIPTION_UI_DESCRIPTION_HPP

#include <svn.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>
#include <ostream>

namespace svn::vst::ui {

// Grid index for single parameter.
struct param_ui_description
{
  std::int32_t row;
  std::int32_t column;
  std::int32_t runtime_param_index;
};

// Complete part with graph and params.
struct part_ui_description
{
  std::int32_t top;
  std::int32_t left;
  std::int32_t width;
  std::int32_t height;
  std::int32_t rows; // Grid rows.
  std::int32_t column; // Own column.
  std::int32_t columns; // Grid columns.
  svn::base::part_ui_color color; // Index into color wheel.
  std::int32_t runtime_part_index; // Part runtime index.
  std::int32_t occupied_cell_count; // Param unique ui indices plus graphs sizes.
  wchar_t const* info; // Extra header info.

  std::int32_t graph_count;
  param_ui_description enabled_param;
  base::graph_descriptor const* graphs;
  std::vector<param_ui_description> params;

  static part_ui_description 
  create(svn::base::topology_info const& topology, std::int32_t runtime_part_index);
};

// Complete controller editor ui.
struct controller_ui_description
{
  std::int32_t width;
  std::int32_t height;
  std::vector<part_ui_description> parts;
  std::vector<std::int32_t> column_widths;

  static controller_ui_description
  create(svn::base::topology_info const& topology);
  void print(svn::base::topology_info const& topology, std::ostream& os);
};

} // namespace svn::vst::ui
#endif // SVN_VST_UI_DESCRIPTION_UI_DESCRIPTION_HPP