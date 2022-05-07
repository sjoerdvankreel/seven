#ifndef SVN_PARAM_PARAM_RANGE_HPP
#define SVN_PARAM_PARAM_RANGE_HPP

#include <svn/param/param_id.hpp>
#include <cstddef>

namespace svn {

struct param_range
{
  param_id const last;
  param_id const first;
  std::size_t const unit_index;
  param_id translate(param_id id) const;
};

inline param_id
param_range::translate(param_id id) const
{

}

} // namespace svn
#endif // SVN_PARAM_PARAM_RANGE_HPP