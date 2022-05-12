#ifndef SVN_TOPOLOGY_PARAM_ID_HPP
#define SVN_TOPOLOGY_PARAM_ID_HPP

#include <cstdint>

namespace svn {

struct param_id_t
{
  enum value: std::uint32_t
  {
    oscillator_level,
    oscillator_detune,
    oscillator_panning,
    oscillator_pulse_width,
    filter_frequency,
    filter_resonance,
    count
  };
};

typedef param_id_t::value param_id;

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_ID_HPP