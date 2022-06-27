#ifndef SVN_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP
#define SVN_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP

#include <svn.base/support/support.hpp>
#include <svn.base/topology/real_bounds.hpp>

#include <cmath>
#include <string>
#include <cstring>
#include <cstdint>

namespace svn::base {

// Optional formatter callbacks for discrete params.
typedef std::wstring (*param_formatter)(std::int32_t val);
typedef bool (*param_parser)(std::wstring const& val, std::int32_t& result);

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
  item_name const* const items; // Pointer to items for a list parameter.
  param_parser const parser; // Parser callback.
  param_formatter const formatter; // Formatter callback.
};

// For ui generator.
struct param_relevance
{
  std::int32_t const if_param;
  std::int32_t const if_value;
};

// For ui generator.
struct param_ui_descriptor
{
  std::int32_t const param_index; // Index within the grid.
  std::int32_t const param_group; // Group together related params within a part.
  param_relevance const* const relevance; // Relevant if all other params relevance.if_param[i] have value relevance.if_value[i].
  std::int32_t const relevance_count;
};

// Describes automation input.
struct param_descriptor
{
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
  param_descriptor(item_name const& static_name, wchar_t const* unit, 
  real_descriptor const& real, param_ui_descriptor const& ui) :
  static_name(static_name), type(param_type::real), unit(unit), real(real), ui(ui) {}

  // Toggle.
  param_descriptor(item_name const& static_name, bool default_, param_ui_descriptor const& ui) :
  static_name(static_name), type(param_type::toggle),
  unit(L""), discrete(discrete_descriptor(0, 1, default_, nullptr, nullptr, nullptr)), ui(ui) {}

  // List/knob list with item list.
  param_descriptor(item_name const& static_name, wchar_t const* unit, bool knob,
  item_name const* const items, std::int32_t count, param_ui_descriptor const& ui) :
  static_name(static_name), type(knob? param_type::knob_list: param_type::list), 
  unit(unit), discrete(discrete_descriptor(0, count - 1, 0, items, nullptr, nullptr)), ui(ui) {}

  // Knob/text.
  param_descriptor(item_name const& static_name, wchar_t const* unit, bool knob, 
  std::int32_t min, std::int32_t max, std::int32_t default_, param_ui_descriptor const& ui) :
  static_name(static_name), type(knob? param_type::knob: param_type::text), 
  unit(unit), discrete(discrete_descriptor(min, max, default_, nullptr, nullptr, nullptr)), ui(ui) {}
   
  // List/knob list with formatter callbacks.
  param_descriptor(item_name const& static_name, wchar_t const* unit, 
  bool knob, std::int32_t min, std::int32_t max, std::int32_t default_, 
  param_formatter formatter, param_parser parser, param_ui_descriptor const& ui) :
  static_name(static_name), type(knob? param_type::knob_list: param_type::list), 
  unit(unit), discrete(discrete_descriptor(min, max, default_, nullptr, parser, formatter)), ui(ui) {}
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP