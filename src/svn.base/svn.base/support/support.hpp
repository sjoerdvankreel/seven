#ifndef SVN_BASE_SUPPORT_SUPPORT_HPP
#define SVN_BASE_SUPPORT_SUPPORT_HPP

#include <cstdint>

namespace svn::base {

// Value container for both real and discrete valued params.
union param_value
{
  float real;
  std::int32_t discrete;

  param_value() : discrete(0) {}
  param_value(float val) : real(val) {}
  param_value(std::int32_t val) : discrete(val) {}
};

// Generic short name/detailed description.
struct item_name
{
  wchar_t const* const short_; // Short name, e.g. "Pw".
  wchar_t const* const detail; // Long name, e.g. "Pulse width".
  item_name(wchar_t const* short_, wchar_t const* detail): short_(short_), detail(detail) {}
};

// Somebody's bound to need this.
inline item_name const note_names[12] =
{
  { L"C", L"C" }, { L"C#", L"C#" }, { L"D", L"D" },
  { L"D#", L"D#" }, { L"E", L"E" }, { L"F", L"F" },
  { L"F#", L"F#" }, { L"G", L"G" }, { L"G#", L"G#" },
  { L"A", L"A" }, { L"A#", L"A#" }, { L"B", L"B" }
};

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_SUPPORT_HPP