#ifndef SVN_PARAM_SYNTH_PARAMS_HPP
#define SVN_PARAM_SYNTH_PARAMS_HPP

#include <svn/param/synth_param.hpp>

namespace svn::synth_params {

inline double unit_gain_default = 1.0;
inline double unit_panning_default = 0.5;

inline synth_param unit1_gain { param_id::unit1_gain, "Gain", "Unit 1 gain", "dB", unit_gain_default };
inline synth_param unit1_panning { param_id::unit1_panning, "Panning", "Unit 1 panning", "%", unit_panning_default };
inline synth_param unit2_gain { param_id::unit2_gain, "Gain", "Unit 2 gain", "dB", unit_gain_default };
inline synth_param unit2_panning { param_id::unit2_panning, "Panning", "Unit 2 panning", "%", unit_panning_default};

} // namespace svn::synth_params
#endif // SVN_PARAM_SYNTH_PARAMS_HPP