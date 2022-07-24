#include <svn.synth/dsp/cv_state.hpp>
#include <svn.base/dsp/support.hpp>

#include <algorithm>

namespace svn::synth {

std::vector<std::vector<std::vector<std::int32_t>>> const cv_state::input_table_in
= svn::base::zip_list_table_init_in(cv_route_input_counts, cv_route_input_op_counts, cv_route_input::count, 0);
std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> const cv_state::input_table_out
= svn::base::zip_list_table_init_out(cv_route_input_counts, cv_route_input_op_counts, cv_route_input::count);
std::vector<std::vector<std::vector<std::int32_t>>> const cv_state::output_table_in
= svn::base::zip_list_table_init_in(cv_route_output_counts, cv_route_output_target_counts, cv_route_output::count, cv_route_param_offset);

cv_state::
cv_state(std::int32_t max_sample_count) :
  envelope(), lfo(), scratch(), scratch_buffer()
{
  std::vector<base::cv_sample> cv(static_cast<std::size_t>(max_sample_count));
  lfo.fill(cv);
  envelope.fill(cv);
  std::int32_t max_param_count = 0;
  for(std::int32_t i = 0; i < cv_route_output::count; i++)
    max_param_count = std::max(max_param_count, cv_route_output_modulated_counts[i]);
  scratch_buffer.resize(static_cast<std::size_t>(max_param_count * max_sample_count));
  for (std::int32_t p = 0; p < max_param_count; p++)
    scratch.push_back(scratch_buffer.data() + p * max_sample_count);
}

base::cv_sample const*
cv_state::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case cv_route_input::off: return nullptr;
  case cv_route_input::lfo: return lfo[index].data();
  case cv_route_input::envelope: return envelope[index].data();
  default: assert(false); return nullptr;
  }
}

double 
cv_state::modulate(voice_input const& input, base::automation_view const& automated,
  std::int32_t const* mapping, cv_route_output route_output, std::int32_t route_index, float const* const*& result) const
{
  double start_time = base::performance_counter();
  std::int32_t input_off = input_table_in[cv_route_input::off][0][0];
  for(std::int32_t p = 0; p < cv_route_output_target_counts[route_output]; p++)
  {
    // cv_route_param_offset = enabled + plot parameters
    std::int32_t output_id = output_table_in[route_output][route_index][p] - cv_route_param_offset;
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      scratch[mapping[p]][s] = automated.get(mapping[p], s).real;
      for(std::int32_t r = 0; r < cv_route_count; r++)
      {
        base::automation_view automation = input.automation.rearrange_params(part_type::cv_route, r);
        if(automation.get(cv_route_param::on, s).discrete == 0) continue;
        for (std::int32_t i = 0; i < cv_route_route_count; i++)
        {
          std::int32_t in = automation.get(i * 3 + cv_route_param_offset, s).discrete;
          if (in == input_off) continue;
          std::int32_t out = automation.get(i * 3 + cv_route_param_offset + 1, s).discrete;
          if (out != output_id) continue;
          float amt = automation.get_as_dsp(i * 3 + cv_route_param_offset + 2, s);
          std::tuple<std::int32_t, std::int32_t, std::int32_t> input_ids(input_table_out[in]);
          float cv = input_buffer(std::get<0>(input_ids), std::get<1>(input_ids))[s].value * amt;
          bool multiply = std::get<2>(input_ids) == cv_route_input_op::multiply;
          if(multiply) scratch[mapping[p]][s] *= cv;
          else scratch[mapping[p]][s] += cv;
        }
      } 
      scratch[mapping[p]][s] = std::clamp(scratch[mapping[p]][s], 0.0f, 1.0f);
    }
  }
  result = scratch.data();
  return base::performance_counter() - start_time;
}

} // namespace svn::synth