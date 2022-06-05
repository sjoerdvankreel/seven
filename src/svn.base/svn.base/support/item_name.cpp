#include <svn.base/support/item_name.hpp>
#include <cassert>

namespace svn::base {

item_name::
item_name(wchar_t const* short_, wchar_t const* detail) :
short_(short_), detail(detail)
{
  assert(short_ != nullptr);
  assert(detail != nullptr);
}

} // namespace svn::base