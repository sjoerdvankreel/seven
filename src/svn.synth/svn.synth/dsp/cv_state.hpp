#ifndef SVN_SYNTH_DSP_CV_STATE_HPP
#define SVN_SYNTH_DSP_CV_STATE_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>

#include <array>
#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::synth {

// Internal cv state.
class cv_state
{
  static std::vector<std::vector<std::vector<std::int32_t>>> const input_table_in;
  static std::vector<std::vector<std::vector<std::int32_t>>> const output_table_in;
  static std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> const input_table_out;

  // Of size max modulated parameter count per part type * max sample count.
  std::vector<float*> scratch;
  std::vector<float> scratch_buffer;
  base::cv_sample const* input_buffer(std::int32_t input, std::int32_t index) const;

public:
  explicit cv_state(std::int32_t max_sample_count);
  std::array<std::vector<base::cv_sample>, envelope_count> envelope;
  std::array<std::vector<base::cv_sample>, voice_lfo_count> voice_lfo;
  double modulate(voice_input const& input, base::automation_view const& automated, std::int32_t const* mapping, 
    cv_route_output route_output, std::int32_t route_index, float const* const*& result) const;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_CV_STATE_HPP