#include <svn.base/support/support.hpp>
#include <immintrin.h>

#include <cassert>
#include <sstream>
#include <algorithm>

namespace svn::base {

std::uint64_t
disable_denormals()
{
  std::uint32_t ftz = _MM_GET_FLUSH_ZERO_MODE();
  std::uint32_t daz = _MM_GET_DENORMALS_ZERO_MODE();
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  return ((static_cast<std::uint64_t>(ftz) << 32) & 0xFFFFFFFF00000000) | daz;
}

void
restore_denormals(std::uint64_t state)
{
  std::uint32_t daz = static_cast<std::uint32_t>(state & 0x00000000FFFFFFFF);
  std::uint32_t ftz = static_cast<std::uint32_t>((state & 0xFFFFFFFF00000000) >> 32);
  _MM_SET_FLUSH_ZERO_MODE(ftz);
  _MM_SET_DENORMALS_ZERO_MODE(daz);
}

bool
list_parser(
  std::string const& val,
  std::vector<std::string> const& names,
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

std::vector<std::vector<std::vector<std::int32_t>>>
zip_list_table_init_in(
  std::int32_t const* counts1, std::int32_t const* counts2, 
  std::int32_t count, std::int32_t offset)
{
  std::int32_t index = offset;
  std::vector<std::vector<std::vector<std::int32_t>>> result;
  for(std::int32_t i = 0; i < count; i++)
  {
    std::vector<std::vector<std::int32_t>> inner1;
    for(std::int32_t j = 0; j < counts1[i]; j++)
    {
      std::vector<std::int32_t> inner2;
      if(counts2[i] == 0)
        inner2.push_back(index++);
      else
        for(std::int32_t k = 0; k < counts2[i]; k++)
          inner2.push_back(index++);
      inner1.push_back(inner2);
    }
    result.push_back(inner1);
  }
  return result;
}

std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>>
zip_list_table_init_out(
  std::int32_t const* counts1, std::int32_t const* counts2, std::int32_t count)
{
  std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> result;
  for (std::int32_t i = 0; i < count; i++)
    for(std::int32_t j = 0; j < counts1[i]; j++)
      if(counts2[i] == 0)
        result.push_back(std::make_tuple(i, j, -1));
      else
        for(std::int32_t k = 0; k < counts2[i]; k++)
          result.push_back(std::make_tuple(i, j, k));
  return result;
}

std::vector<std::string>
list_names(char const* name, std::int32_t count, list_item_info info)
{
  std::vector<std::string> result;
  for (std::int32_t i = 0; i < count; i++)
  {
    std::string item = name + std::string(" ") + std::to_string(i + 1);
    if (info != nullptr)
    {
      char const* extra = info(i);
      if (extra != nullptr)
        item = std::string(extra) + " " + name;
    }
    result.push_back(item);
  }
  return result;
}

std::vector<std::string>
multi_list_names(
  char const* const* names,
  std::int32_t const* counts, std::int32_t count)
{
  std::vector<std::string> result;
  for(std::int32_t i = 0; i < count; i++)
    for(std::int32_t j = 0; j < counts[i]; j++)
    {
      std::string name = names[i];
      if(counts[i] > 1) name += " " + std::to_string(j + 1);
      result.push_back(name);
    }
  return result;
} 

std::vector<std::string>
zip_list_names(
  char const* const* names1, list_item_info const* infos, std::int32_t const* counts1,
  char const* const* const* names2, std::int32_t const* counts2,
  std::int32_t count)
{
  std::vector<std::string> result;
  for (std::int32_t i = 0; i < count; i++)
    for (std::int32_t j = 0; j < counts1[i]; j++)
      if (counts2[i] == 0)
      {
        std::string name = names1[i];
        if (counts1[i] > 1) name += " " + std::to_string(j + 1);
        result.push_back(name);
      }
      else for(std::int32_t k = 0; k < counts2[i]; k++)
      {
        std::string name = names1[i];
        if(counts1[i] > 1)
        { 
          name += " " + std::to_string(j + 1);
          if (infos != nullptr && infos[i] != nullptr)
          {
            char const* extra = infos[i](j);
            if(extra != nullptr)
              name = std::string(extra) + " " + names1[i];
          }
        }
        result.push_back(name + " " + names2[i][k]);
      }
  return result; 
}

std::vector<std::pair<std::int32_t, std::int32_t>>
beat_synced_timesig(std::int32_t sig_count, bool with_zero)
{
  // Make all signatures.
  std::vector<std::pair<std::int32_t, std::int32_t>> all;
  if(with_zero) 
    all.push_back(std::make_pair(0, 0));
  for (std::int32_t num = 1; num <= sig_count; num++)
    for (std::int32_t den = 1; den <= sig_count; den++)
      all.push_back(std::make_pair(num, den));

  // Filter out equivalents (e.g. 2/4 == 4/8).
  bool seen_this;
  std::vector<float> seen;
  float const epsilon = 1.0e-3f;
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

std::vector<std::string>
beat_synced_timesig_names(
  std::vector<std::pair<std::int32_t, std::int32_t>> const& timesig)
{
  std::vector<std::string> result;
  for (std::size_t i = 0; i < timesig.size(); i++)
    if (timesig[i].second == 0)
      result.push_back("0");
    else
    result.push_back(std::to_string(timesig[i].first) + "/" + std::to_string(timesig[i].second));
  return result;
} 
 
} // namespace svn::base