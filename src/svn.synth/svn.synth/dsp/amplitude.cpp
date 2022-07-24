#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/amplitude.hpp>
#include <svn.synth/topology/topology.hpp>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

double
amplitude::process_block(voice_input const& input, base::cv_sample const* env1_cv,
  base::audio_sample32 const* audio_in, base::audio_sample32* audio_out)
{
  double start_time = performance_counter();
  automation_view automation(input.automation.rearrange_params(part_type::amplitude, 0));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  { 
    float level = automation.get_as_dsp(amplitude_param::level, s);
    float panning = automation.get_as_dsp(amplitude_param::pan, s);
    float env1_uni = !env1_cv[s].bipolar? env1_cv[s].value: (env1_cv[s].value + 1.0f) * 0.5f;
    float final_amp = level * env1_uni;
    float left = audio_in[s].left * final_amp * (1.0f - panning);
    float right = audio_in[s].right * final_amp * panning;
    audio_out[s] = sanity_audio(audio_sample32({left, right}));
  }
  return performance_counter() - start_time;
}

} // namespace svn::synth