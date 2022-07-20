#ifndef SVN_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP
#define SVN_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP

#include <svn.base/support/support.hpp>
#include <svn.base/topology/real_bounds.hpp>

#include <cmath>
#include <string>
#include <cstring>
#include <cstdint>

namespace svn::base {

// Discrete param types.
struct param_type_t { enum value { real, toggle, knob, text, list, knob_list, count }; };
typedef param_type_t::value param_type;

// Real valued specific data.
struct real_descriptor
{
  float const default_; // Normalized default.
  std::int32_t const precision; // Value formatting precision.
  real_bounds const dsp; // Dsp range.
  real_bounds const display; // Display range.
};

// Discrete valued specific data.
struct discrete_descriptor
{
  std::int32_t const min;
  std::int32_t const max;
  std::int32_t const default_;
  std::vector<std::wstring> const* const items; // Items for a list parameter.
};

// For ui generator.
struct param_relevance
{
  std::int32_t const if_param;
  std::vector<std::int32_t> const if_values; // Relevant if any of these values.
};

// For ui generator.
struct param_ui_descriptor
{
  bool const control_font_small; // For list params.
  bool const label_font_small; // For any labels.
  std::int32_t const param_index; // Relative index within the grid.
  std::int32_t const param_group; // Group together related params within a part.
  param_relevance const* const relevance; // Relevant if all other params relevance.if_param[i] have any of values relevance.if_values[i].
  std::int32_t const relevance_count;
};

// For selector.
inline param_ui_descriptor constexpr param_no_ui = { false, false, -1, -1, nullptr, 0 };

// Describes automation input.
struct param_descriptor
{
  std::string const guid; // Unique param id for serialization.
  item_name const static_name; // Static name, e.g. "Frequency", "Resonance".
  param_type const type; // Parameter type.
  wchar_t const* const unit; // Parameter unit, e.g. "dB", "Hz".
  param_ui_descriptor const ui; // For ui generator. 
  union 
  {
    real_descriptor const real; // Real valued specific data.
    discrete_descriptor const discrete; // Discrete valued specific data.
  };

  bool parse(wchar_t const* buffer, param_value& val) const;
  std::size_t format(param_value val, wchar_t* buffer, std::size_t size) const;

  // Real.
  param_descriptor(std::string const& guid, item_name const& static_name, 
  wchar_t const* unit, real_descriptor const& real, param_ui_descriptor const& ui) :
  guid(guid), static_name(static_name), type(param_type::real), unit(unit), real(real), ui(ui) {}

  // Toggle.
  param_descriptor(std::string const& guid, item_name const& static_name, 
  bool default_, param_ui_descriptor const& ui) :
  guid(guid), static_name(static_name), type(param_type::toggle),
  unit(L""), discrete(discrete_descriptor(0, 1, default_,  nullptr)), ui(ui) {}

  // Knob/text.
  param_descriptor(std::string const& guid, item_name const& static_name, wchar_t const* unit,
  bool knob, std::int32_t min, std::int32_t max, std::int32_t default_, param_ui_descriptor const& ui) :
  guid(guid), static_name(static_name), type(knob? param_type::knob: param_type::text),
  unit(unit), discrete(discrete_descriptor(min, max, default_, nullptr)), ui(ui) {}
   
  // List/knob list with item list.
  param_descriptor(std::string const& guid, item_name const& static_name, wchar_t const* unit, 
  bool knob, std::vector<std::wstring> const* items, std::int32_t default_, param_ui_descriptor const& ui) :
  guid(guid), static_name(static_name), type(knob? param_type::knob_list: param_type::list),
  unit(unit), discrete(discrete_descriptor(0, static_cast<std::int32_t>(items->size() - 1), default_, items)), ui(ui) {}
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP