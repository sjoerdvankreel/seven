#ifndef SVN_PARAM_SYNTH_PARAMS_HPP
#define SVN_PARAM_SYNTH_PARAMS_HPP

#include <svn/param/param_id.hpp>
#include <svn/param/param_info.hpp>
#include <svn/param/synth_param.hpp>
#include <vector>

namespace svn::synth_params {

inline param_info unit_level_info = { L"Level", L"dB", 1.0 };
inline param_info unit_detune_info = { L"Detune", L"%", 0.5 };
inline param_info unit_panning_info = { L"Panning", L"%", 0.5 };
inline param_info unit_pulse_width_info = { L"Pulse width", L"%", 0.0 };

inline synth_param unit1_level { param_id::unit1_level, L"Unit 1 level", unit_level_info };
inline synth_param unit1_detune { param_id::unit1_detune, L"Unit 1 detune", unit_detune_info };
inline synth_param unit1_panning { param_id::unit1_panning, L"Unit 1 panning", unit_panning_info };
inline synth_param unit1_pulse_width { param_id::unit1_pulse_width, L"Unit 1 pulse width", unit_pulse_width_info };
inline synth_param unit2_level { param_id::unit2_level, L"Unit 2 level", unit_level_info };
inline synth_param unit2_detune { param_id::unit2_detune, L"Unit 2 detune", unit_detune_info };
inline synth_param unit2_panning { param_id::unit2_panning, L"Unit 2 panning", unit_panning_info };
inline synth_param unit2_pulse_width { param_id::unit2_pulse_width, L"Unit 2 pulse width", unit_pulse_width_info };

inline std::vector<synth_param> all = 
{
  unit1_level,
  unit1_detune,
  unit1_panning,
  unit1_pulse_width,
  unit2_level,
  unit2_detune,
  unit2_panning,
  unit2_pulse_width,
};

} // namespace svn::synth_params
#endif // SVN_PARAM_SYNTH_PARAMS_HPP