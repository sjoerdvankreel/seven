#include <svn.synth/dsp/cv_state.hpp>
#include <svn.base/dsp/support.hpp>

namespace svn::synth {

std::vector<std::vector<std::int32_t>> const cv_state::input_table_in
= svn::base::multi_list_table_init_in(cv_input_counts, cv_route_input::count);
std::vector<std::pair<std::int32_t, std::int32_t>> const cv_state::input_table_out
= svn::base::multi_list_table_init_out(cv_input_counts, cv_route_input::count);
std::vector<std::vector<std::vector<std::int32_t>>> const cv_state::output_table_in
= svn::base::zip_list_table_init_in(cv_output_counts, cv_output_target_counts, cv_route_output::count, cv_route_param_offset);

cv_state::
cv_state(std::int32_t max_sample_count) :
  envelope(), voice_lfo(), scratch(), scratch_buffer()
{
  std::vector<base::cv_sample> cv(static_cast<std::size_t>(max_sample_count));
  envelope.fill(cv);
  voice_lfo.fill(cv);
  std::int32_t max_outputs = *std::max_element(cv_output_target_counts, cv_output_target_counts + cv_route_output::count);
  scratch_buffer.resize(static_cast<std::size_t>(max_outputs * max_sample_count));
  for (std::int32_t p = 0; p < max_outputs; p++)
    scratch.push_back(scratch_buffer.data() + p * max_sample_count);
}

base::cv_sample const*
cv_state::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case cv_route_input::off: return nullptr;
  case cv_route_input::vlfo: return voice_lfo[index].data();
  case cv_route_input::envelope: return envelope[index].data();
  default: assert(false); return nullptr;
  }
}

float const* const* 
cv_state::modulate(voice_input const& input, base::automation_view const& automated,
  std::int32_t const* mapping, cv_route_output route_output, std::int32_t route_index) const
{
  std::int32_t input_off = input_table_in[cv_route_input::off][0];
  base::automation_view automation = input.automation.rearrange_params(part_type::cv_route, 0);
  for(std::int32_t p = 0; p < cv_output_target_counts[route_output]; p++)
  {
    // cv_route_param_offset = plot parameters
    std::int32_t output_id = output_table_in[route_output][route_index][p] - cv_route_param_offset;
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      scratch[p][s] = automated.get(mapping[p], s).real;
      for (std::int32_t i = 0; i < cv_route_count; i++)
      {
        std::int32_t in = automation.get(i * 3 + cv_route_param_offset, s).discrete;
        if (in == input_off) continue;
        std::int32_t out = automation.get(i * 3 + cv_route_param_offset + 1, s).discrete;
        if (out != output_id) continue;
        float amt = automation.get(i * 3 + cv_route_param_offset + 2, s).real;
        std::pair<std::int32_t, std::int32_t> input_ids(input_table_out[in]);
        scratch[p][s] = base::modulate(scratch[p][s], input_buffer(input_ids.first, input_ids.second)[s], amt);
      }
    }
  }
  return scratch.data();
}

} // namespace svn::synth