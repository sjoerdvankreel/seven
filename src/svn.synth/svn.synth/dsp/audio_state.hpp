#ifndef SVN_SYNTH_DSP_AUDIO_STATE_HPP
#define SVN_SYNTH_DSP_AUDIO_STATE_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.synth/topology/topology.hpp>

#include <array>
#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::synth {

// Internal audio outputs.
class audio_state
{
  static std::vector<std::vector<std::int32_t>> const input_table_in;
  static std::vector<std::vector<std::int32_t>> const output_table_in;
  static std::vector<std::pair<std::int32_t, std::int32_t>> const input_table_out;

  std::vector<base::audio_sample32> scratch;
  base::audio_sample32 const* input_buffer(std::int32_t input, std::int32_t index) const;

public:
  std::vector<base::audio_sample32> amplitude;
  std::array<std::vector<base::audio_sample32>, filter_count> filter;
  std::array<std::vector<base::audio_sample32>, oscillator_count> oscillator;
  
  explicit audio_state(std::int32_t max_sample_count);
  double mix(voice_input const& input, cv_state const& cv, audio_route_output route_output,
    std::int32_t route_index, svn::base::audio_sample32 const*& result, double& mod_time);
};

} // namespace svn::base
#endif // SVN_SYNTH_DSP_AUDIO_STATE_HPP