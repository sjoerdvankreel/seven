#ifndef SVN_BASE_RUNTIME_RUNTIME_PARAM_HPP
#define SVN_BASE_RUNTIME_RUNTIME_PARAM_HPP

#include <string>
#include <cstdint>

namespace svn::base {

struct runtime_param
{
  std::int32_t part_index; // Index into runtime_part array, e.g. 0/1/2/3 when 2 units + 2 filters.
  std::wstring const runtime_name; // Runtime parameter name, e.g. "Filter 2 frequency".
  struct runtime_part const* part; // Pointer to runtime part, e.g. filter 1 descriptor.
  struct param_descriptor const* descriptor; // Pointer to static parameter information.

  runtime_param(
    std::int32_t part_index,
    std::wstring const& runtime_name,
    struct runtime_part const* part,
    struct param_descriptor const* descriptor);
};

} // namespace svn::base
#endif // SVN_BASE_RUNTIME_RUNTIME_PARAM_HPP