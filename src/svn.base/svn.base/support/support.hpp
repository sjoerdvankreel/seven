#ifndef SVN_BASE_SUPPORT_SUPPORT_HPP
#define SVN_BASE_SUPPORT_SUPPORT_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <utility>

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
  char const* const short_; // Short name, e.g. "Pw".
  char const* const detail; // Long name, e.g. "Pulse width".
  item_name(char const* short_, char const* detail): short_(short_), detail(detail) {}
};

typedef char const* (*list_item_info)(std::int32_t index);

// Somebody's bound to need this.
inline std::vector<std::string> const note_names = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

// Toggle daz/ftz.
std::uint64_t disable_denormals();
void restore_denormals(std::uint64_t state);

// Returns index of item.
bool
list_parser(
  std::string const& val, 
  std::vector<std::string> const& names,
  std::int32_t& result);

// E.g. "Osc", 2 -> "Osc 1", "Osc 2".
std::vector<std::string>
list_names(char const* name, std::int32_t count, list_item_info info);

// Pass list of names with count per name.
// E.g. ["Osc", "Flt"], [2, 1] -> ["Osc 1", "Osc 2", "Flt"].
std::vector<std::string>
multi_list_names(
  char const* const* names,
  std::int32_t const* counts, std::int32_t count);

// Pass list of names with count per name plus second list specific to item in first.
// E.g. ["Osc", "Flt"], [2, 1], ["Amp", "Pan"], ["Freq", "Res"] 
// -> ["Osc 1 Amp", "Osc 1 Pan", "Osc 2 Amp", "Osc 2 Pan", "Flt Freq", "Flt Res"] 
std::vector<std::string>
zip_list_names(
  char const* const* names1, list_item_info const* infos, std::int32_t const* counts1,
  char const* const* const* names2, std::int32_t const* counts2,
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
std::vector<std::string>
beat_synced_timesig_names(
  std::vector<std::pair<std::int32_t, std::int32_t>> const& timesig);

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_SUPPORT_HPP