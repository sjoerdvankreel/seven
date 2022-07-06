#include <svn.base/support/support.hpp>

#include <cassert>
#include <sstream>
#include <algorithm>

namespace svn::base {

bool
list_parser(
  std::wstring const& val,
  std::vector<std::wstring> const& names,
  std::int32_t& result)
{
  for(std::size_t i = 0; i < names.size(); i++)
    if (val == names[i])
    {
      result = static_cast<std::int32_t>(i);
      return true;
    }
  return false;    
}

std::vector<std::vector<std::int32_t>> 
multi_list_table_init_in(
  std::int32_t const* counts, std::int32_t count)
{
  std::int32_t index = 0;
  std::vector<std::vector<std::int32_t>> result;
  for (std::int32_t i = 0; i < count; i++)
  {
    std::vector<std::int32_t> inner;
    for(std::int32_t j = 0; j < counts[i]; j++)
      inner.push_back(index++);
    result.push_back(inner);
  }
  return result;
}

std::vector<std::pair<std::int32_t, std::int32_t>>
multi_list_table_init_out(
  std::int32_t const* counts, std::int32_t count)
{
  std::vector<std::pair<std::int32_t, std::int32_t>> result;
  for(std::int32_t i = 0; i < count; i++)
    for(std::int32_t j = 0; j < counts[i]; j++)
      result.push_back(std::make_pair(i, j));
  return result;
}

std::wstring 
multi_list_formatter(
  std::int32_t val, wchar_t const** names,
  std::int32_t const* counts, std::int32_t count)
{
  std::int32_t group_start = 0;
  for (std::int32_t i = 0; i < count; i++)
    if (val >= group_start && val < group_start + counts[i])
      if(counts[i] == 1)
        return names[i];
      else
        return std::wstring(names[i]) + L" " + std::to_wstring(val - group_start + 1);
    else group_start += counts[i];
  assert(false);
  return L"";
}

bool 
multi_list_parser(
  std::wstring const& val, wchar_t const** names,
  std::int32_t const* counts, std::int32_t count,
  std::int32_t& result)
{
  std::int32_t group_start = 0;
  for(std::int32_t i = 0; i < count; i++)
    if(val.starts_with(names[i]))
      if (counts[i] == 1)
      {
        if(val != names[i]) return false;
        result = group_start;
        return true;
      }
      else
      {
        std::wstring num = val.substr(std::wcslen(names[i]) + 1);
        std::wstringstream wss(num);
        wss >> result;
        result -= 1;
        if(!(0 <= result && result < counts[i])) return false;
        result += group_start;
        return true;
      }
    else group_start += counts[i];
  return false;
}

std::vector<std::pair<std::int32_t, std::int32_t>>
beat_synced_timesig(std::int32_t sig_count)
{
  // Make all signatures.
  std::vector<std::pair<std::int32_t, std::int32_t>> all;
  all.push_back(std::make_pair(0, 0));
  for (std::int32_t num = 1; num <= sig_count; num++)
    for (std::int32_t den = 1; den <= sig_count; den++)
      all.push_back(std::make_pair(num, den));

  // Filter out equivalents (e.g. 2/4 == 4/8).
  bool seen_this;
  std::vector<float> seen;
  float const epsilon = 1.0e-3;
  std::vector<std::pair<std::int32_t, std::int32_t>> result;
  for (std::size_t i = 0; i < all.size(); i++)
  {
    seen_this = false;
    float val = all[i].second == 0 ? 0.0f : static_cast<float>(all[i].first) / all[i].second;
    for (std::size_t j = 0; j < seen.size(); j++)
      if (val - epsilon <= seen[j] && seen[j] <= val + epsilon)
      {
        seen_this = true;
        break;
      }
    if (seen_this) continue;
    seen.push_back(val);
    result.push_back(all[i]);
  }

  // Sort ascending.
  std::sort(result.begin(), result.end(), [](auto const& l, auto const& r) -> bool
  { 
    float lval = l.second == 0 ? 0.0f : static_cast<float>(l.first) / l.second;
    float rval = r.second == 0 ? 0.0f : static_cast<float>(r.first) / r.second;
    return lval < rval;
  });
  return result;
}

std::vector<float>
beat_synced_timesig_values(
  std::vector<std::pair<std::int32_t, std::int32_t>> const& timesig)
{
  std::vector<float> result;
  for (std::size_t i = 0; i < timesig.size(); i++)
    if(timesig[i].second == 0)
      result.push_back(0.0f);
    else
      result.push_back(static_cast<float>(timesig[i].first) / timesig[i].second);
  return result;
}

std::vector<std::wstring>
beat_synced_timesig_names(
  std::vector<std::pair<std::int32_t, std::int32_t>> const& timesig)
{
  std::vector<std::wstring> result;
  for (std::size_t i = 0; i < timesig.size(); i++)
    if (timesig[i].second == 0)
      result.push_back(L"0");
    else
    result.push_back(std::to_wstring(timesig[i].first) + L"/" + std::to_wstring(timesig[i].second));
  return result;
}
 
} // namespace svn::base