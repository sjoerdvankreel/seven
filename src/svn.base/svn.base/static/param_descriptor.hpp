#ifndef SVN_BASE_STATIC_PARAM_DESCRIPTOR_HPP
#define SVN_BASE_STATIC_PARAM_DESCRIPTOR_HPP

#include <svn.base/static/param_bounds.hpp>
#include <svn.base/support/item_name.hpp>
#include <svn.base/support/param_value.hpp>
#include <cstdint>

namespace svn::base {
  
struct param_type_t { enum value { real, list, toggle, discrete, count }; };
typedef param_type_t::value param_type;
  
// Describes automation input.
struct param_descriptor
{
  param_type const type; // Parameter type.
  wchar_t const* const unit; // Parameter unit, e.g. "dB", "Hz".
  item_name const static_name; // Static name, e.g. "Frequency", "Resonance".
  item_name const* const list; // Pointer to items for a list parameter.
  param_bounds const dsp, display; // Dsp and display ranges for real parameters.
  param_value const min, max, default_; // Range for discrete parameters. 0..1 for real parameters.

  param_descriptor(
    item_name const& static_name, bool default_);
  param_descriptor(
    item_name const& static_name, param_type type,
    item_name const* list, std::int32_t count);
  param_descriptor(
    item_name const& static_name, wchar_t const* unit,
    std::int32_t default_, std::int32_t min, std::int32_t max);
  param_descriptor(
    item_name const& static_name, wchar_t const* unit, float default_,
    param_bounds const& dsp, param_bounds const& display);

  param_value to_display(param_value val) const;
  param_value from_display(param_value val) const;
  bool parse(wchar_t const* buffer, param_value& val) const;
  std::size_t format(param_value val, wchar_t* buffer, std::size_t size) const;
};

} // namespace svn::base
#endif // SVN_BASE_STATIC_PARAM_DESCRIPTOR_HPP