#include <svn.base/static/part_descriptor.hpp>
#include <cassert>

namespace svn::base {

part_descriptor::
part_descriptor(
  std::int32_t type,
  item_name const& static_name, std::int32_t part_count,
  param_descriptor const* params, std::int32_t param_count) :
type(type), static_name(static_name), part_count(part_count),
param_count(param_count), params(params)
{  
  assert(part_count > 0);
  assert(param_count > 0);
  assert(params != nullptr);
}

} // namespace svn::base