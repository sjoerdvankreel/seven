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
  std::int32_t max_note_events = 0; // Not the same as polyphony.
  std::vector<runtime_part> parts; // Runtime part descriptor array, e.g. osc 1, osc 2, filter 1, filter 2.
  std::vector<runtime_param> params; // Runtime parameter descriptor array, e.g. osc 1 wave, osc 1 amp, osc 2 wave, etc.
  std::vector<std::vector<std::int32_t>> bounds; // Runtime parameter bounds, e.g. bounds[part_type::osc][1] points to osc 2 wave.
  std::int32_t max_ui_height; // For ui generator. 
  std::int32_t const* ui_order; // For ui generator. Part types mixed with ui_order_output_params. Ui generation goes top-down first, then left-right.
  std::int32_t input_param_count = 0; // Runtime input params.
  std::int32_t output_param_count = 0; // Runtime output params.

  // Params contains interior pointers into parts.
  runtime_topology() = default;
  runtime_topology(runtime_topology const&) = delete;

  void init_defaults(union param_value* state) const;

  // Output params must follow input params.
  static std::unique_ptr<runtime_topology> create(
    struct part_descriptor const* static_parts, std::int32_t part_count,     
    std::int32_t max_notes, std::int32_t max_ui_height, std::int32_t const* ui_order);
};

} // namespace svn::base
#endif // SVN_BASE_RUNTIME_RUNTIME_TOPOLOGY_HPP