#ifndef SVN_BASE_SUPPORT_SUPPORT_HPP
#define SVN_BASE_SUPPORT_SUPPORT_HPP

#include <cstdint>
#include <immintrin.h>

namespace svn::base {

inline std::uint64_t
disable_denormals()
{
  std::uint32_t ftz = _MM_GET_FLUSH_ZERO_MODE();
  std::uint32_t daz = _MM_GET_DENORMALS_ZERO_MODE();
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  return ((static_cast<std::uint64_t>(ftz) << 32) & 0xFFFFFFFF00000000) | daz;
}

inline void
restore_denormals(std::uint64_t state)
{
  std::uint32_t daz = static_cast<std::uint32_t>(state & 0x00000000FFFFFFFF);
  std::uint32_t ftz = static_cast<std::uint32_t>((state & 0xFFFFFFFF00000000) >> 32);
  _MM_SET_FLUSH_ZERO_MODE(ftz);
  _MM_SET_DENORMALS_ZERO_MODE(daz);
}

// Value container for both real and discrete valued params.
union param_value
{
  float real;
  std::int32_t discrete;

  param_value() : discrete(0) {}
  explicit param_value(float val) : real(val) {}
  explicit param_value(std::int32_t val) : discrete(val) {}
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