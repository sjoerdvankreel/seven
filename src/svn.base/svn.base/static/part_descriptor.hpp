#ifndef SVN_BASE_STATIC_PART_DESCRIPTOR_HPP
#define SVN_BASE_STATIC_PART_DESCRIPTOR_HPP

#include <svn.base/support/item_name.hpp>
#include <cstdint>

namespace svn::base {

struct part_descriptor
{
  std::int32_t const type; // Type id, e.g. Osc, Filter.
  item_name const static_name; // Static name, e.g. "Osc", "Filter".
  std::int32_t const part_count; // Part count of this type, e.g. 2 filters.
  std::int32_t const param_count; // Parameter count for a part of this type, e.g. 2: frequency, resonance.
  struct param_descriptor const* const params; // Pointer to parameter descriptor array.

  part_descriptor(
    std::int32_t type,
    item_name const& static_name, std::int32_t part_count,
    param_descriptor const* params, std::int32_t param_count);
};

} // namespace svn::base
#endif // SVN_BASE_STATIC_PART_DESCRIPTOR_HPP