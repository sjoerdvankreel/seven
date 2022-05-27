#ifndef SVN_BASE_RUNTIME_RUNTIME_TOPOLOGY_HPP
#define SVN_BASE_RUNTIME_RUNTIME_TOPOLOGY_HPP

#include <svn.base/runtime/runtime_part.hpp>
#include <svn.base/runtime/runtime_param.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace svn::base {

struct runtime_topology
{
  std::int32_t const real_count; // Real valued runtime parameter count.
  std::int32_t const discrete_count; // Discrete valued runtime parameter count.
  std::vector<runtime_part> const parts; // Runtime part descriptor array, e.g. osc 1, osc 2, filter 1, filter 2.
  std::vector<runtime_param> const params; // Runtime parameter descriptor array, e.g. osc 1 wave, osc 1 amp, osc 2 wave, etc.
  std::vector<std::vector<std::int32_t>> const bounds; // Runtime parameter bounds, e.g. bounds[part_type::osc][1] points to osc 2 wave.

  runtime_topology(
    std::vector<runtime_part> const& parts,
    std::vector<runtime_param> const& params,
    std::vector<std::vector<std::int32_t>> const& bounds,
    std::int32_t real_count,
    std::int32_t discrete_count);

  void init_defaults(union param_value* state) const;
  static std::unique_ptr<runtime_topology> create(
    struct part_descriptor const* static_parts, std::int32_t count);
};

} // namespace svn::base
#endif // SVN_BASE_RUNTIME_RUNTIME_TOPOLOGY_HPP