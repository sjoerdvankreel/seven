#ifndef SVN_SUPPORT_PARAM_TRANSFORM_HPP
#define SVN_SUPPORT_PARAM_TRANSFORM_HPP

#include <svn/support/topo_static.hpp>
#include <cstdint>
#include <cstddef>
#include <cassert>

namespace svn {

param_value param_to_display(param_info const& info, param_value val);
param_value param_from_display(param_info const& info, param_value val);
bool param_parse(param_info const& info, wchar_t const* buffer, param_value& val);
void param_format(param_info const& info, param_value val, wchar_t* buffer, std::size_t size);

} // namespace svn
#endif // SVN_SUPPORT_PARAM_TRANSFORM_HPP