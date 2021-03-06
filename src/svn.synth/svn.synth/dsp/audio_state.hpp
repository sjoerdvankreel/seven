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

struct audio_route_indices
{
  std::int32_t bank_index;
  std::int32_t route_index;
  std::pair<std::int32_t, std::int32_t> input_ids;
};

// Internal audio outputs.
class audio_state
{
  static std::vector<std::vector<std::int32_t>> const input_table_in;
  static std::vector<std::vector<std::int32_t>> const output_table_in;
  static std::vector<std::pair<std::int32_t, std::int32_t>> const input_table_out;

  std::vector<base::audio_sample32> scratch;

  std::int32_t _relevant_indices_count = 0;
  std::array<base::automation_view, audio_route_count> _bank_automation;
  // Of size audio banks * audio routes per bank.
  // On begin of mix(), we calculate relevant stuff for the current runtime part.
  std::array<audio_route_indices, audio_route_count * audio_route_route_count> _relevant_indices;

  base::audio_sample32 const* input_buffer(std::int32_t input, std::int32_t index) const;

public:
  std::vector<base::audio_sample32> amplitude;
  std::array<std::vector<base::audio_sample32>, filter_count> filter;
  std::array<std::vector<base::audio_sample32>, oscillator_count> oscillator;
  
  explicit audio_state(std::int32_t max_sample_count);
  double mix(voice_input const& input, cv_state& cv, audio_route_output route_output,
    std::int32_t route_index, svn::base::audio_sample32 const*& result, double& cv_time);
};

} // namespace svn::base
#endif // SVN_SYNTH_DSP_AUDIO_STATE_HPP