#ifndef SVN_PARAM_SYNTH_PARAMS_HPP
#define SVN_PARAM_SYNTH_PARAMS_HPP

#include <svn/param/synth_param.hpp>

namespace svn::synth_params {

inline double unit_gain_default = 1.0;
inline double unit_panning_default = 0.5;

inline synth_param unit1_gain { param_id::unit1_gain, unit_gain_default, "Gain", "Unit 1 gain" };
inline synth_param unit1_panning { param_id::unit1_panning, unit_panning_default, "Panning", "Unit 1 panning" };
inline synth_param unit2_gain { param_id::unit2_gain, unit_gain_default, "Gain", "Unit 2 gain" };
inline synth_param unit2_panning { param_id::unit2_panning, unit_panning_default, "Panning", "Unit 2 panning" };

} // namespace svn::synth_params
#endif // SVN_PARAM_SYNTH_PARAMS_HPP