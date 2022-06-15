#include <svn.base/topology/runtime_part.hpp>
#include <cassert>

namespace svn::base {

runtime_part::
runtime_part(
  std::int32_t type_index,
  std::wstring const& runtime_name,
  std::int32_t runtime_param_start,
  struct part_descriptor const* descriptor):
type_index(type_index), runtime_name(runtime_name),
runtime_param_start(runtime_param_start), descriptor(descriptor)
{
  assert(type_index >= 0);
  assert(descriptor != nullptr);
  assert(runtime_param_start >= 0);
}

} // namespace svn::base
