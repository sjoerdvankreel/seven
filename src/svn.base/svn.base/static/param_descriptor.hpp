#ifndef SVN_BASE_STATIC_PARAM_DESCRIPTOR_HPP
#define SVN_BASE_STATIC_PARAM_DESCRIPTOR_HPP

#include <svn.base/static/param_bounds.hpp>
#include <svn.base/support/item_name.hpp>
#include <svn.base/support/param_value.hpp>
#include <cstdint>

namespace svn::base {
  
struct param_type_t { enum value { real, list, toggle, discrete, discrete_list, discrete_text, count }; };
typedef param_type_t::value param_type;
  
// Describes automation input.
struct param_descriptor
{
  param_type const type; // Parameter type.
  wchar_t const* const unit; // Parameter unit, e.g. "dB", "Hz".
  item_name const static_name; // Static name, e.g. "Frequency", "Resonance".
  item_name const* const list; // Pointer to items for a list parameter.
  std::int32_t const precision; // Real value formatting precision.
  param_bounds const dsp, display; // Dsp and display ranges for real parameters.
  param_value const min, max, default_; // Range for discrete parameters. 0..1 for real parameters.

  bool ui_edit_font_small; // For ui generator. Set to true if value gets large.
  std::int32_t ui_param_index; // For ui generator. Index within the grid.
  std::int32_t ui_param_group; // For ui generator. Group together related params within a part.
  std::int32_t ui_relevant_if_param; // For ui generator. This parameter is visible if other param P has value V. -1 if not applicable.
  std::int32_t ui_relevant_if_value; // For ui generator. This parameter is visible if other param P has value V.

  param_descriptor(item_name const& static_name, bool default_, 
    std::int32_t ui_param_index, std::int32_t ui_param_group, bool ui_edit_font_small, 
    std::int32_t ui_relevant_if_param, std::int32_t ui_relevant_if_value);
  param_descriptor(item_name const& static_name, 
    param_type type, item_name const* list, std::int32_t count, 
    std::int32_t ui_param_index, std::int32_t ui_param_group, bool ui_edit_font_small,
    std::int32_t ui_relevant_if_param, std::int32_t ui_relevant_if_value);
  param_descriptor(item_name const& static_name, 
    param_type type, wchar_t const* unit, std::int32_t default_, 
    std::int32_t min, std::int32_t max, 
    std::int32_t ui_param_index, std::int32_t ui_param_group, bool ui_edit_font_small,
    std::int32_t ui_relevant_if_param, std::int32_t ui_relevant_if_value);
  param_descriptor(item_name const& static_name, 
    wchar_t const* unit, float default_, std::int32_t precision, 
    param_bounds const& dsp, param_bounds const& display,
    std::int32_t ui_param_index, std::int32_t ui_param_group, bool ui_edit_font_small,
    std::int32_t ui_relevant_if_param, std::int32_t ui_relevant_if_value);

  param_value to_display(param_value val) const;
  param_value from_display(param_value val) const;
  bool parse(wchar_t const* buffer, param_value& val) const;
  std::size_t format(param_value val, wchar_t* buffer, std::size_t size) const;
};

} // namespace svn::base
#endif // SVN_BASE_STATIC_PARAM_DESCRIPTOR_HPP