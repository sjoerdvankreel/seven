#ifndef SVN_BASE_STATIC_PART_DESCRIPTOR_HPP
#define SVN_BASE_STATIC_PART_DESCRIPTOR_HPP

#include <svn.base/support/item_name.hpp>
#include <cstdint>

namespace svn::base {

struct part_descriptor
{
  bool output; // Part contains output parameters.
  std::int32_t const type; // Type id, e.g. Osc, Filter.
  item_name const static_name; // Static name, e.g. "Osc", "Filter".
  std::int32_t const part_count; // Part count of this type, e.g. 2 filters.
  std::int32_t const param_count; // Parameter count for a part of this type, e.g. 2: frequency, resonance.
  struct param_descriptor const* const params; // Pointer to parameter descriptor array.
  std::int32_t const ui_control_columns; // For ui generator. Number of parameters stacked left-to-right.
  std::int32_t const ui_control_enabled; // For ui generator. Index into params which identifies the part on/off switch, or -1 if always on.

  part_descriptor(
    std::int32_t type, bool output,
    item_name const& static_name, std::int32_t part_count,
    param_descriptor const* params, std::int32_t param_count,
    std::int32_t ui_control_columns, std::int32_t ui_control_enabled);
};

} // namespace svn::base
#endif // SVN_BASE_STATIC_PART_DESCRIPTOR_HPP