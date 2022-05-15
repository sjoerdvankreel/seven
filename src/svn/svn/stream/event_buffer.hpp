#ifndef SVN_EVENT_EVENT_BUFFER_HPP
#define SVN_EVENT_EVENT_BUFFER_HPP

#include <svn/event/note_buffer.hpp>
#include <svn/event/automation_buffer.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
struct event_buffer
{
  note_buffer notes;
  std::int32_t automation_count;
  automation_buffer<sample_type> const* automation;
};

} // namespace svn
#endif // SVN_EVENT_EVENT_BUFFER_HPP