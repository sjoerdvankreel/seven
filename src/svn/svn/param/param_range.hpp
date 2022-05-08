#ifndef SVN_PARAM_PARAM_RANGE_HPP
#define SVN_PARAM_PARAM_RANGE_HPP

#include <svn/param/param_id.hpp>
#include <cstdint>

namespace svn {

struct param_range
{
  std::int32_t const offset; 
  param_id translate(param_id id) const;
  param_range(std::int32_t unit_index, param_id first, param_id last);
};

inline param_range::
param_range(std::int32_t unit_index, param_id first, param_id last):
offset(unit_index * (static_cast<std::int32_t>(last) - static_cast<std::int32_t>(first) + 1)) {}

inline param_id 
param_range::translate(param_id id) const
{

}

} // namespace svn
#endif // SVN_PARAM_PARAM_RANGE_HPP