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
  envelope(), lfo(), _scratch(), _scratch_buffer(), 
  _relevant_indices(), _bank_automation(),
  velocity(static_cast<std::size_t>(max_sample_count))
{
  std::vector<base::cv_sample> cv(static_cast<std::size_t>(max_sample_count));
  lfo.fill(cv);
  envelope.fill(cv);
  std::int32_t max_param_count = 0;
  for(std::int32_t i = 0; i < cv_route_output::count; i++)
    max_param_count = std::max(max_param_count, cv_route_output_modulated_counts[i]);
  _scratch_buffer.resize(static_cast<std::size_t>(max_param_count * max_sample_count));
  for (std::int32_t p = 0; p < max_param_count; p++)
    _scratch.push_back(_scratch_buffer.data() + p * max_sample_count);
}

base::cv_sample const*
cv_state::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case cv_route_input::off: return nullptr;
  case cv_route_input::lfo: return lfo[index].data();
  case cv_route_input::velocity: return velocity.data();
  case cv_route_input::envelope: return envelope[index].data();
  default: assert(false); return nullptr;
  }
}

double 
cv_state::modulate(
  voice_input const& input, base::automation_view const& automated, std::int32_t const* mapping, 
  cv_route_output route_output, std::int32_t route_index, float const* const*& result)
{
  double start_time = base::performance_counter();
  std::int32_t input_off = input_table_in[cv_route_input::off][0][0];
  std::int32_t output_off = output_table_in[cv_route_output::off][0][0];

  // Set scratch to current values.
  for (std::int32_t p = 0; p < cv_route_output_target_counts[route_output]; p++)
  {
    // cv_route_param_offset = enabled + plot parameters
    std::int32_t output_id = output_table_in[route_output][route_index][p] - cv_route_param_offset;
    automated.get_real(mapping[p], _scratch[mapping[p]], input.sample_count);
  }

  // Find out relevant modulation targets.
  // Note: discrete automation per block, not per sample!
  _relevant_indices_count = 0;
  for(std::int32_t r = 0; r < cv_route_count; r++)
  {
    _bank_automation[r] = input.automation.rearrange_params(part_type::cv_route, r);
    if(_bank_automation[r].get_discrete(cv_route_param::on, 0) == 0) continue;
    for (std::int32_t i = 0; i < cv_route_route_count; i++)
    {
      std::int32_t input_id = _bank_automation[r].get_discrete(i * 3 + cv_route_param_offset, 0);
      if(input_id == input_off) continue;
      for (std::int32_t p = 0; p < cv_route_output_target_counts[route_output]; p++)
      {
        std::int32_t output_id = output_table_in[route_output][route_index][p] - cv_route_param_offset;
        if(output_id == output_off) continue;
        if(_bank_automation[r].get_discrete(i * 3 + cv_route_param_offset + 1, 0) != output_id) continue;
        route_indices indices;
        indices.bank_index = r;
        indices.route_index = i;
        indices.target_index = p;
        indices.input_ids = input_table_out[input_id];
        _relevant_indices[_relevant_indices_count++] = indices;
      }
    }
  }

  // Apply modulation.
  for (std::int32_t m = 0; m < _relevant_indices_count; m++)
  {
    route_indices indices = _relevant_indices[m];
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      float amt = _bank_automation[indices.bank_index].get_real_dsp(indices.route_index * 3 + cv_route_param_offset + 2, s);
      float cv = input_buffer(std::get<0>(indices.input_ids), std::get<1>(indices.input_ids))[s].value * amt;
      bool multiply = std::get<2>(indices.input_ids) == cv_route_input_op::multiply;
      if (multiply) _scratch[mapping[indices.target_index]][s] *= cv;
      else _scratch[mapping[indices.target_index]][s] += cv;
      _scratch[mapping[indices.target_index]][s] = std::clamp(_scratch[mapping[indices.target_index]][s], 0.0f, 1.0f);
    }
  }

  result = _scratch.data();
  return base::performance_counter() - start_time;
}

} // namespace svn::synth