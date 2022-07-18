#ifndef SVN_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP
#define SVN_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP

#include <svn.base/support/support.hpp>
#include <svn.base/topology/param_descriptor.hpp>
#include <cstdint>

namespace svn::base {

// Optional single graph within 1 part.
struct graph_descriptor
{
  std::int32_t const type; // Graph type.
  std::int32_t const row; // Rows in ui grid.
  std::int32_t const column; // Columns in ui grid.
  std::int32_t const row_span; // Row count in ui grid.
  std::int32_t const column_span; // Column count in ui grid.
  wchar_t const* const description;
};

// For ui generator.
struct part_ui_descriptor
{
  std::int32_t const part_index; // Index within the grid.
  std::int32_t const param_columns; // Number of parameters stacked left-to-right.
  std::int32_t const enabled_param; // Index into own params which identifies the part on/off switch, or -1 if always on.
  std::int32_t const selector_param; // Index into selector params which identifies the part index switch param if this part type has more then 1 runtime part, or -1 if part count is 1.
  wchar_t const* const info; // Extra info for part header ui.
};

// For selector.
inline part_ui_descriptor constexpr part_no_ui = { -1, -1, -1, -1, nullptr };

// Static part info.
struct part_descriptor
{
  std::string const guid; // Unique part id for serialization.
  item_name const static_name; // Static name, e.g. "Osc", "Filter".
  std::int32_t const type; // Type id, e.g. Osc, Filter.
  bool const output; // Part contains output parameters.
  bool const selector; // Part contains selector parameters (e.g. switch Osc1/2). At most 1 part containing selector params.
  std::int32_t const part_count; // Part count of this type, e.g. 2 filters.
  param_descriptor const* const params; // Pointer to parameter descriptor array.
  std::int32_t const param_count; // Parameter count for a part of this type, e.g. 2: frequency, resonance.
  graph_descriptor const* const graphs; // Optional graph infos for this part.
  std::int32_t const graph_count; // Number of graphs for this part type.
  part_ui_descriptor const ui; // For ui generator.
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP