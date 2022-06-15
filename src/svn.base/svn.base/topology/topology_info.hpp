#ifndef SVN_BASE_TOPOLOGY_TOPOLOGY_INFO_HPP
#define SVN_BASE_TOPOLOGY_TOPOLOGY_INFO_HPP

#include <svn.base/support/param_value.hpp>
#include <svn.base/topology/part_descriptor.hpp>
#include <svn.base/topology/param_descriptor.hpp>

#include <string>
#include <vector>
#include <cstdint>

namespace svn::base {

// For ui generator.
struct topology_ui_info
{
  std::int32_t max_height; // Max controller ui height.
  std::vector<std::vector<std::int32_t>> param_dependencies; // Toggles visibility.
};

// Runtime part info (e.g. osc 2).
struct part_info
{
  std::int32_t const type_index; // Part index within this type, e.g. 0/1 in case of 2 filters.
  std::wstring const runtime_name; // Runtime name, e.g. "Filter 1", "Filter 2". Equals static name when part count is 1.
  std::int32_t const runtime_param_start; // Index into topology.
  part_descriptor const* const descriptor; // Pointer to static part information.
};

// Runtime param info (e.g. filter 2 resonance).
struct param_info
{
  std::int32_t const part_index; // Index into runtime_part array, e.g. 0/1/2/3 when 2 oscillators + 2 filters.
  std::wstring const runtime_name; // Runtime parameter name, e.g. "Filter 2 frequency".
  param_descriptor const* const descriptor; // Pointer to static parameter information.
};

// Runtime part and parameter layout.
struct topology_info
{
  topology_ui_info ui; // For ui generator.
  part_descriptor const* static_parts; // Static description of audio processor.
  std::int32_t static_part_count; // Part count in static description.
  std::int32_t max_note_events = 0; // Not necessarily the same as polyphony.
  std::int32_t input_param_count = 0; // Runtime input params.
  std::int32_t output_param_count = 0; // Runtime output params.  

  std::vector<part_info> parts; // Runtime part descriptor array, e.g. osc 1, osc 2, filter 1, filter 2.
  std::vector<param_info> params; // Runtime parameter descriptor array, e.g. osc 1 wave, osc 1 amp, osc 2 wave, etc.
  std::vector<std::vector<std::int32_t>> part_bounds; // Runtime part bounds, e.g. bounds[part_type::osc][1] indexes osc 2 part.
  std::vector<std::vector<std::int32_t>> param_bounds; // Runtime parameter bounds, e.g. bounds[part_type::osc][1] indexes osc 2 wave param.

  // Output params must follow input params.
  void init_defaults(param_value* state) const;
  static topology_info create(part_descriptor const* static_parts,
    std::int32_t part_count, std::int32_t max_notes, std::int32_t max_ui_height);
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_TOPOLOGY_INFO_HPP