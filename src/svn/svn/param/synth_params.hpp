#ifndef SVN_PARAM_SYNTH_PARAMS_HPP
#define SVN_PARAM_SYNTH_PARAMS_HPP

#include <svn/param/param_id.hpp>
#include <svn/param/param_info.hpp>
#include <svn/param/synth_param.hpp>
#include <vector>

namespace svn::synth_params {

inline param_info unit_gain_info = { L"Gain", L"dB", 1.0 };
inline param_info unit_panning_info = { L"Panning", L"%", 0.5 };

inline synth_param unit1_gain { param_id::unit1_gain, L"Unit 1 gain", unit_gain_info };
inline synth_param unit1_panning { param_id::unit1_panning, L"Unit 1 panning", unit_panning_info };
inline synth_param unit2_gain { param_id::unit2_gain, L"Unit 2 gain", unit_gain_info };
inline synth_param unit2_panning { param_id::unit2_panning, L"Unit 2 panning", unit_panning_info };

inline std::vector<synth_param> all = 
{
  unit1_gain,
  unit1_panning,
  unit2_gain,
  unit2_panning
};

} // namespace svn::synth_params
#endif // SVN_PARAM_SYNTH_PARAMS_HPP