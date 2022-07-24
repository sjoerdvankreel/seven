#include <svn.synth/dsp/audio_state.hpp>
#include <svn.base/dsp/support.hpp>

namespace svn::synth {

std::vector<std::vector<std::int32_t>> const audio_state::input_table_in
= svn::base::multi_list_table_init_in(audio_route_input_counts, audio_route_input::count);
std::vector<std::vector<std::int32_t>> const audio_state::output_table_in
= svn::base::multi_list_table_init_in(audio_route_output_counts, audio_route_output::count);
std::vector<std::pair<std::int32_t, std::int32_t>> const audio_state::input_table_out
= svn::base::multi_list_table_init_out(audio_route_input_counts, audio_route_input::count);

audio_state::
audio_state(std::int32_t max_sample_count):  
oscillator(), filter()
{
  std::vector<base::audio_sample32> audio(static_cast<std::size_t>(max_sample_count));
  scratch = audio;
  amplitude = audio;
  filter.fill(audio);
  oscillator.fill(audio);
}

base::audio_sample32 const*
audio_state::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case audio_route_input::off: return nullptr;
  case audio_route_input::filter: return filter[index].data();
  case audio_route_input::osc: return oscillator[index].data();
  default: assert(false); return nullptr;
  }
}

double
audio_state::mix(voice_input const& input, audio_route_output route_output, 
  std::int32_t route_index, svn::base::audio_sample32 const*& result)
{
  double start_time = base::performance_counter();
  std::int32_t input_off = input_table_in[audio_route_input::off][0];
  std::int32_t output_id = output_table_in[route_output][route_index];
  for(std::int32_t s = 0; s < input.sample_count; s++)
  {
    scratch[s] = { 0.0f, 0.0f };
    for(std::int32_t r = 0; r < audio_route_count; r++)
    {
      base::automation_view automation = input.automation.rearrange_params(part_type::audio_route, r);
      if(automation.get_discrete(audio_route_param::on, s) == 0) continue;
      for (std::int32_t i = 0; i < audio_route_route_count; i++)
      {
        std::int32_t in = automation.get_discrete(audio_route_param_offset + i * 3, s);
        if (in == input_off) continue;
        std::int32_t out = automation.get_discrete(audio_route_param_offset + i * 3 + 1, s);
        if (out != output_id) continue;
        float amt = automation.get_real_dsp(audio_route_param_offset + i * 3 + 2, s);
        std::pair<std::int32_t, std::int32_t> input_ids(input_table_out[in]);
        scratch[s] = base::sanity_audio(scratch[s] + input_buffer(input_ids.first, input_ids.second)[s] * amt);
      }
    }
  }
  result = scratch.data();
  return base::performance_counter() - start_time;
}

} // namespace svn::synth