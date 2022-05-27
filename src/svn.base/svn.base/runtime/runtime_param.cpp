#include <svn.base/runtime/runtime_param.hpp>
#include <cassert>

namespace svn::base {

runtime_param::
runtime_param(
  std::int32_t part_index, std::wstring const& runtime_name,
  struct runtime_part const* part, struct param_descriptor const* descriptor):
part_index(part_index), runtime_name(runtime_name),
part(part), descriptor(descriptor)
{
  assert(part_index >= 0);
  assert(part != nullptr);
  assert(descriptor != nullptr);
}

} // namespace svn::base