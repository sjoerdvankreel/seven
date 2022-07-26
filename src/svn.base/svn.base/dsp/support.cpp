#include <svn.base/dsp/support.hpp>

namespace svn::base {

note_to_frequency_table_init const
note_to_frequency_table_init::init;

note_to_frequency_table_init::
note_to_frequency_table_init():
table()
{
  for(std::int32_t o = 0; o < octave_count; o++)
    for(std::int32_t n = 0; n < note_count; n++)
      for(std::int32_t c = 0; c < cent_count; c++)
        table[o * (note_count * cent_count) + n * cent_count + c] = note_to_frequency(o * note_count + n + c / 100.0f);
}

} // namespace svn::base