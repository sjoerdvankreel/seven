#ifndef SVN_TOPOLOGY_PARAM_BOUNDS_HPP
#define SVN_TOPOLOGY_PARAM_BOUNDS_HPP

namespace svn {

template <typename sample_type>
struct param_bounds
{
  sample_type min;
  sample_type max;
  sample_type default_;
};

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_BOUNDS_HPP