#ifndef SVN_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP
#define SVN_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP

#include <svn.base/support/support.hpp>
#include <svn.base/topology/graph_descriptor.hpp>
#include <svn.base/topology/param_descriptor.hpp>
#include <cstdint>

namespace svn::base {

// For ui generator.
struct part_ui_descriptor
{
  std::int32_t const color_index; // Index into color wheel.
  std::int32_t const param_columns; // Number of parameters stacked left-to-right.
  std::int32_t const enabled_param; // Index into params which identifies the part on/off switch, or -1 if always on.
  graph_descriptor const* const graph; // Optional graph info for this part.
};

// Static part info.
struct part_descriptor
{
  item_name const static_name; // Static name, e.g. "Osc", "Filter".
  std::int32_t const type; // Type id, e.g. Osc, Filter.
  bool const output; // Part contains output parameters.
  std::int32_t const part_count; // Part count of this type, e.g. 2 filters.
  param_descriptor const* const params; // Pointer to parameter descriptor array.
  std::int32_t const param_count; // Parameter count for a part of this type, e.g. 2: frequency, resonance.
  part_ui_descriptor const ui; // For ui generator.
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP