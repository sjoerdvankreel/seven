#ifndef SVN_BASE_TOPOLOGY_GRAPH_DESCRIPTOR_HPP
#define SVN_BASE_TOPOLOGY_GRAPH_DESCRIPTOR_HPP

#include <cstdint>

namespace svn::base {

struct graph_placement_t { enum value { left, right, top, bottom }; };
typedef graph_placement_t::value graph_placement;

// For ui generator.
struct graph_descriptor
{
  std::int32_t rows; // Rows in ui grid.
  std::int32_t columns; // Columns in ui grid.
  graph_placement placement; // Positioning.
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_GRAPH_DESCRIPTOR_HPP