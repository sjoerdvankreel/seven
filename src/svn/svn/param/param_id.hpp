#ifndef SVN_PARAM_PARAM_ID_HPP
#define SVN_PARAM_PARAM_ID_HPP

#include <cstdint>

namespace svn {

struct param_id_t
{
  enum value: std::uint32_t
  {
    unit1_level,
    unit1_detune,
    unit1_panning,
    unit1_pulse_width,
    unit2_level,
    unit2_detune,
    unit2_panning,
    unit2_pulse_width,
    count
  };
};

typedef param_id_t::value param_id;

} // namespace svn
#endif // SVN_PARAM_PARAM_ID_HPP