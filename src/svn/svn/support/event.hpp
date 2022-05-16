#ifndef SVN_SUPPORT_EVENT_HPP
#define SVN_SUPPORT_EVENT_HPP

#include <svn/support/topology.hpp>
#include <cstdint>

namespace svn {

inline std::int32_t constexpr note_off = -1;

struct automation_buffer
{
  param_id param;
  float const* samples;
};

struct note_buffer
{
  std::int32_t count;
  std::int32_t const* midi;
  std::int32_t const* indices;
};

struct event_buffer
{
  note_buffer notes;
  std::int32_t automation_count;
  automation_buffer const* automation;
};

} // namespace svn
#endif // SVN_SUPPORT_EVENT_HPP