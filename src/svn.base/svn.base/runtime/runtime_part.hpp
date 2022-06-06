#ifndef SVN_BASE_RUNTIME_RUNTIME_PART_HPP
#define SVN_BASE_RUNTIME_RUNTIME_PART_HPP

#include <string>
#include <cstdint>

namespace svn::base {

struct runtime_part
{
  std::int32_t const type_index; // Part index within this type, e.g. 0/1 in case of 2 filters.
  std::wstring const runtime_name; // Runtime name, e.g. "Filter 1", "Filter 2". Equals static name when part count is 1.
  std::int32_t const runtime_param_start; // Index into topology.
  struct part_descriptor const* const descriptor; // Pointer to static part information.

  runtime_part(
    std::int32_t type_index, 
    std::wstring const& runtime_name, 
    std::int32_t runtime_param_start,
    struct part_descriptor const* descriptor);
}; 

} // namespace svn::base
#endif // SVN_BASE_RUNTIME_RUNTIME_PART_HPP