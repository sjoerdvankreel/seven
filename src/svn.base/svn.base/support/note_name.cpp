#include <svn.base/support/note_name.hpp>

namespace svn::base {

item_name const
note_names[note_name::count] =
{
  { L"C", L"C" },
  { L"C#", L"C#" },
  { L"D", L"D" },
  { L"D#", L"D#" },
  { L"E", L"E" },
  { L"F", L"F" },
  { L"F#", L"F#" },
  { L"G", L"G" },
  { L"G#", L"G#" },
  { L"A", L"A" },
  { L"A#", L"A#" },
  { L"B", L"B" }
};

} // namespace svn::base