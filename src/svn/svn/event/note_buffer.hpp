#ifndef SVN_EVENT_NOTE_BUFFER_HPP
#define SVN_EVENT_NOTE_BUFFER_HPP

#include <cstdint>

namespace svn {

struct note_buffer
{
  std::int32_t const* midi;
  std::int32_t const* indices;
};

} // namespace svn
#endif // SVN_EVENT_NOTE_BUFFER_HPP