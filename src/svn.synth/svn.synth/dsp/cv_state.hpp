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

struct cv_route_indices
{
  std::int32_t bank_index;
  std::int32_t route_index;
  std::int32_t target_index;
  std::tuple<std::int32_t, std::int32_t, std::int32_t> input_ids;
};

// Internal cv state.
// Handles both modulation and transformation from [0, 1] to dsp range (e.g. 20-20000 for filter freq).
// All modulation is done in [0, 1], final result is clipped.
class cv_state
{
  static std::vector<std::vector<std::vector<std::int32_t>>> const input_table_in;
  static std::vector<std::vector<std::vector<std::int32_t>>> const output_table_in;
  static std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> const input_table_out;

  // Of size max parameter count per part type * max sample count.
  std::vector<float*> _scratch;
  std::vector<float> _scratch_buffer;
  
  base::topology_info const* const _topology;
  std::int32_t _relevant_indices_count = 0;
  std::array<base::automation_view, cv_route_count> _bank_automation;
  // Of size cv banks * cv routes per bank.
  // On begin of transform(), we calculate relevant stuff for the current runtime part.
  std::array<cv_route_indices, cv_route_count * cv_route_route_count> _relevant_indices;

  base::cv_sample const* input_buffer(std::int32_t input, std::int32_t index) const;

public:
  std::vector<base::cv_sample> velocity;
  std::array<std::vector<base::cv_sample>, lfo_count> lfo;
  std::array<std::vector<base::cv_sample>, envelope_count> envelope;
  cv_state(base::topology_info const* topology, std::int32_t max_sample_count);

  // Note: CV in [0, 1] on input, parameter dsp range on output.
  // Apply optional modulation and rescale to dsp range.
  double transform(
    voice_input const& input, base::automation_view const& automated, cv_route_output route_output, 
    std::int32_t part_index, std::int32_t const* output_mapping, float const* const*& result);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_CV_STATE_HPP