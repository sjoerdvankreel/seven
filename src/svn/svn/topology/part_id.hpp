#ifndef SVN_TOPOLOGY_PART_ID_HPP
#define SVN_TOPOLOGY_PART_ID_HPP

#include <cstdint>

namespace svn {

struct part_id_t
{
  enum value: std::uint32_t
  {
    oscillator,
    filter,
    count
  };
};

typedef part_id_t::value part_id;

} // namespace svn
#endif // SVN_TOPOLOGY_PART_ID_HPP