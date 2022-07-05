#include <svn.base/support/support.hpp>

#include <cassert>
#include <sstream>

namespace svn::base {

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

std::vector<std::pair<std::int32_t, std::int32_t>>
generate_beat_synced_timesig(std::int32_t sig_count)
{
  std::vector<std::pair<std::int32_t, std::int32_t>> result;
  result.push_back(std::make_pair(0, 0));
  for(std::int32_t num = 1; num <= sig_count; num++)
    for(std::int32_t den = 1; den <= sig_count; den++)
      result.push_back(std::make_pair(num, den));
  
}

std::vector<std::pair<std::int32_t, std::int32_t>>
generate_beat_synced_timesig_names(std::int32_t sig_count);


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
 
} // namespace svn::base