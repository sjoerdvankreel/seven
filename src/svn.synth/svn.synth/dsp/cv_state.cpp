#include <svn.synth/dsp/cv_state.hpp>
#include <svn.base/dsp/support.hpp>
#include <svn.base/support/platform.hpp>

#include <algorithm>

namespace svn::synth {

std::vector<std::vector<std::vector<std::int32_t>>> const cv_state::input_table_in
= svn::base::zip_list_table_init_in(cv_route_input_counts, cv_route_input_op_counts, cv_route_input::count, 0);
std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> const cv_state::input_table_out
= svn::base::zip_list_table_init_out(cv_route_input_counts, cv_route_input_op_counts, cv_route_input::count);
std::vector<std::vector<std::vector<std::int32_t>>> const cv_state::output_table_in
= svn::base::zip_list_table_init_in(cv_route_output_counts, cv_route_output_target_counts, cv_route_output::count, cv_route_param_offset);

cv_state::
cv_state(base::topology_info const* topology, std::int32_t max_sample_count) :
_scratch(), _scratch_buffer(), _topology(topology),
_relevant_indices_count(0), _bank_automation(), _relevant_indices(),
velocity(static_cast<std::size_t>(max_sample_count)), lfo(), envelope()
{
  std::vector<base::cv_sample> cv(static_cast<std::size_t>(max_sample_count));
  lfo.fill(cv);
  envelope.fill(cv);
  std::int32_t max_param_count = 0;
  // Make room for every part type, even if it's never modulated.
  // This is because we also handle transformation to dsp range here,
  // for example for the envelope parameters which are not modulated.
  for(std::int32_t i = 0; i < part_type::count; i++)
    max_param_count = std::max(max_param_count, topology->parts[i].descriptor->param_count);
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

// Note: we skip over discrete params, 
// they are not taken into account in result, 
// but their positions are taken (so we waste some space).
double 
cv_state::transform_unmodulated(base::automation_view const& automated, 
  part_type type, std::int32_t sample_count, float const* const*& result)
{
  double start_time = base::performance_counter();
  for(std::int32_t p = 0; p < _topology->static_parts[type].param_count; p++)
    if(_topology->static_parts[type].params[p].type == base::param_type::real)
    {
      automated.automation_real(p, _scratch[p], sample_count);
      automated.transform_real(p, _scratch[p], sample_count);
    }
  result = _scratch.data();
  return base::performance_counter() - start_time;
}

double 
cv_state::transform_modulated(
  voice_input const& input, base::automation_view const& automated, cv_route_output route_output, 
  std::int32_t part_index, std::int32_t const* output_mapping, float const* const*& result)
{
  double start_time = base::performance_counter();
  std::int32_t input_off = input_table_in[cv_route_input::off][0][0];
  std::int32_t output_off = output_table_in[cv_route_output::off][0][0];

  // Set scratch to current values in [0, 1].
  // cv_route_param_offset = enabled + plot parameters
  for (std::int32_t p = 0; p < cv_route_output_target_counts[route_output]; p++)
    automated.automation_real(output_mapping[p], _scratch[output_mapping[p]], input.sample_count);

  // Find out relevant modulation targets.
  // Note: discrete automation per block, not per sample!
  _relevant_indices_count = 0;
  for(std::int32_t r = 0; r < cv_route_count; r++)
  {
    _bank_automation[r] = input.automation.rearrange_params(part_type::cv_route, r);
    if(_bank_automation[r].automation_discrete(cv_route_param::on, 0) == 0) continue;
    for (std::int32_t i = 0; i < cv_route_route_count; i++)
    {
      std::int32_t input_id = _bank_automation[r].automation_discrete(i * 3 + cv_route_param_offset, 0);
      if(input_id == input_off) continue;
      for (std::int32_t p = 0; p < cv_route_output_target_counts[route_output]; p++)
      {
        std::int32_t output_id = output_table_in[route_output][part_index][p] - cv_route_param_offset;
        if(output_id == output_off) continue;
        if(_bank_automation[r].automation_discrete(i * 3 + cv_route_param_offset + 1, 0) != output_id) continue;
        cv_route_indices indices;
        indices.bank_index = r;
        indices.route_index = i;
        indices.target_index = p;
        indices.input_ids = input_table_out[input_id];
        _relevant_indices[_relevant_indices_count++] = indices;
      }
    }
  }

  // Apply modulation (can scale beyond [0, 1], we apply clipping just before transform to dsp domain).
  for (std::int32_t m = 0; m < _relevant_indices_count; m++)
  {
    cv_route_indices indices = _relevant_indices[m];
    float* cv_output = _scratch[output_mapping[indices.target_index]];
    bool multiply = std::get<2>(indices.input_ids) == cv_route_input_op::multiply;
    base::cv_sample const* cv_input = input_buffer(std::get<0>(indices.input_ids), std::get<1>(indices.input_ids));
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      float amt = _bank_automation[indices.bank_index].get_real_dsp(indices.route_index * 3 + cv_route_param_offset + 2, s);
      float cv = cv_input[s].value * amt;
      if (multiply) cv_output[s] *= cv;
      else cv_output[s] += cv;
      if (m == _relevant_indices_count - 1)
        cv_output[s] = std::clamp(cv_output[s], 0.0f, 1.0f);
    }
  }

  // Scale [0, 1] to dsp. Note: scale everything, also stuff that wasn't modulated.
  // On end of transform(), each series should be in it's own domain for dsp processing.
  // cv_route_param_offset = enabled + plot parameters.
  for (std::int32_t p = 0; p < cv_route_output_target_counts[route_output]; p++)
    automated.transform_real(output_mapping[p], _scratch[output_mapping[p]], input.sample_count);

  result = _scratch.data();
  return base::performance_counter() - start_time;
}

} // namespace svn::synth