#include <svn.base/runtime/runtime_part.hpp>
#include <cassert>

namespace svn::base {

runtime_part::
runtime_part(
  std::int32_t type_index,
  std::wstring const& runtime_name,
  struct part_descriptor const* descriptor):
type_index(type_index), runtime_name(runtime_name), descriptor(descriptor)
{
  assert(type_index >= 0);
  assert(descriptor != nullptr);
}

} // namespace svn::base
