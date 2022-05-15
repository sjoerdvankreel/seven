#ifndef SVN_TOPOLOGY_PARAM_LISTS_HPP
#define SVN_TOPOLOGY_PARAM_LISTS_HPP

#include <svn/topology/param_topology.hpp>

namespace svn {

struct oscillator_type_t { enum value { blep, blamp, dsf, count }; };
typedef oscillator_type_t oscillator_type;
extern param_item const oscillator_types[];

struct filter_type_t { enum value { ladder, state_variable, count }; };
typedef filter_type_t filter_type;
extern param_item const filter_types[];

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_LISTS_HPP