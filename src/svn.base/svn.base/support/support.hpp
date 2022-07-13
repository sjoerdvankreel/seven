#ifndef SVN_BASE_SUPPORT_SUPPORT_HPP
#define SVN_BASE_SUPPORT_SUPPORT_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <immintrin.h>

namespace svn::base {

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

// Returns index of item.
bool
list_parser(
  std::wstring const& val, 
  std::vector<std::wstring> const& names,
  std::int32_t& result);

// Pass list of names with count per name.
// E.g. ["Osc", "Flt"], [2, 1] -> ["Osc 1", "Osc 2", "Flt"].
std::vector<std::wstring>
multi_list_names(
  wchar_t const* const* names, 
  std::int32_t const* counts, std::int32_t count);

// Pass list of names with count per name plus second list specific to item in first.
// E.g. ["Osc", "Flt"], [2, 1], ["Amp", "Pan"], ["Freq", "Res"] 
// -> ["Osc 1 Amp", "Osc 1 Pan", "Osc 2 Amp", "Osc 2 Pan", "Flt Freq", "Flt Res"] 
std::vector<std::wstring>
zip_list_names(
  wchar_t const* const* names1, std::int32_t const* counts1,
  wchar_t const* const* const* names2, std::int32_t const* counts2,
  std::int32_t count);

// (Part x, index y) <-> multi list index.
std::vector<std::vector<std::int32_t>> 
multi_list_table_init_in(
  std::int32_t const* counts, std::int32_t count);
std::vector<std::pair<std::int32_t, std::int32_t>>
multi_list_table_init_out(
  std::int32_t const* counts, std::int32_t count);

// (Part x, index y, param z) <-> zip list index.
std::vector<std::vector<std::vector<std::int32_t>>>
zip_list_table_init_in(
  std::int32_t const* counts1, std::int32_t const* counts2, 
  std::int32_t count, std::int32_t offset);
std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>>
zip_list_table_init_out(
  std::int32_t const* counts1, std::int32_t const* counts2, std::int32_t count);

// 0/0, 1/4, 2/4, 3/4, 1/3, 2/3 etc.
std::vector<std::pair<std::int32_t, std::int32_t>>
beat_synced_timesig(
  std::int32_t sig_count, bool with_zero);
std::vector<float>
beat_synced_timesig_values(
  std::vector<std::pair<std::int32_t, std::int32_t>> const& timesig);
std::vector<std::wstring>
beat_synced_timesig_names(
  std::vector<std::pair<std::int32_t, std::int32_t>> const& timesig);

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_SUPPORT_HPP