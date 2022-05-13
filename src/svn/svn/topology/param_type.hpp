#ifndef SVN_TOPOLOGY_PARAM_TYPE_HPP
#define SVN_TOPOLOGY_PARAM_TYPE_HPP

namespace svn {

struct param_type_t
{
  enum value
  {
    real,
    list,
    toggle,
    discrete
  };
};

typedef param_type_t::value param_type;

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_TYPE_HPP