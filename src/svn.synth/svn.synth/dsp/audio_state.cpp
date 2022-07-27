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
oscillator(), filter(), _relevant_indices(), _bank_automation()
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
audio_state::mix(
  voice_input const& input, cv_state& cv, audio_route_output route_output,
  std::int32_t route_index, svn::base::audio_sample32 const*& result, double& mod_time)
{
  double start_mod_time = mod_time;
  double start_time = base::performance_counter();
  std::int32_t input_off = input_table_in[audio_route_input::off][0];
  std::int32_t output_id = output_table_in[route_output][route_index];
  std::memset(scratch.data(), 0, input.sample_count * sizeof(base::audio_sample32));

  // Find out relevant audio targets.
  // Note: discrete automation per block, not per sample!
  _relevant_indices_count = 0;
  for (std::int32_t r = 0; r < audio_route_count; r++)
  {
    _bank_automation[r] = input.automation.rearrange_params(part_type::audio_route, r);
    if (_bank_automation[r].input_discrete(audio_route_param::on, 0) == 0) continue;
    for (std::int32_t i = 0; i < audio_route_route_count; i++)
    {
      std::int32_t input_id = _bank_automation[r].input_discrete(i * 3 + audio_route_param_offset, 0);
      if (input_id == input_off) continue;
      std::int32_t this_output_id = _bank_automation[r].input_discrete(i * 3 + 1 + audio_route_param_offset, 0);
      if (output_id != this_output_id) continue;
      audio_route_indices indices;
      indices.bank_index = r;
      indices.route_index = i;
      indices.input_ids = input_table_out[input_id];
      _relevant_indices[_relevant_indices_count++] = indices;
    }
  }

  // Apply routing.
  float const* const* modulated;
  std::int32_t previous_bank_index = -1;
  for (std::int32_t a = 0; a < _relevant_indices_count; a++)
  {
    audio_route_indices indices = _relevant_indices[a];
    base::audio_sample32 const* audio = input_buffer(indices.input_ids.first, indices.input_ids.second);
    if(indices.bank_index != previous_bank_index)
      mod_time += cv.modulate(input, _bank_automation[indices.bank_index], cv_route_audio_mapping, cv_route_output::audio, indices.bank_index, modulated);
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      float amt = _bank_automation[indices.bank_index].get_modulated_dsp(audio_route_param_offset + indices.route_index * 3 + 2, s, modulated);
      scratch[s] = base::sanity_audio(scratch[s] + audio[s] * std::clamp(amt, 0.0f, 1.0f));
    }
  }

  result = scratch.data();
  return base::performance_counter() - start_time - (mod_time - start_mod_time);
}

} // namespace svn::synth