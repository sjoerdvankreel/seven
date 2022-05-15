#ifndef SVN_TOPOLOGY_PARAM_ID_HPP
#define SVN_TOPOLOGY_PARAM_ID_HPP

namespace svn {

struct param_id_t
{
  enum value
  {
    oscillator_enabled,
    oscillator_type,
    oscillator_level,
    oscillator_detune,
    oscillator_panning,
    oscillator_pulse_width,

    filter_enabled,
    filter_type,
    filter_frequency,
    filter_resonance,

    count
  };
};

typedef param_id_t::value param_id;

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_ID_HPP