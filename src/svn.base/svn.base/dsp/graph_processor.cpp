#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/graph_processor.hpp>
#include <cmath>

namespace svn::base {

// Given parameter state and sample rate, setup automation and run some dsp code.
template <class T>
std::vector<T> const&
graph_processor<T>::process_dsp(param_value const* state, float sample_rate, float bpm)
{
  _automation.clear();
  _automation_buffer.clear();
  std::int32_t samples = sample_count(state, sample_rate, bpm);
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    for (std::int32_t s = 0; s < samples; s++)
      _automation_buffer.push_back(state[p]);
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    _automation.push_back(_automation_buffer.data() + static_cast<std::size_t>(p * samples));

  _raw_data.clear();
  _raw_data.resize(samples);

  block_input input;
  input.bpm = bpm;
  input.note_count = 0;
  input.notes = nullptr;
  input.stream_position = 0;
  input.sample_count = samples;
  input.automation = _automation.data();

  // This produces the graph specific dsp data.
  std::uint64_t denormal_state = disable_denormals();
  process_dsp_core(input, _raw_data.data(), sample_rate, bpm);
  restore_denormals(denormal_state);
  return _raw_data;
}

template <class T>
std::vector<graph_point> const&
graph_processor<T>::plot(param_value const* state, 
  float sample_rate, float bpm, std::int32_t width, std::int32_t height, bool& bipolar)
{
  _plot_data.clear();
  process_dsp(state, sample_rate, bpm);
  dsp_to_plot(state, _raw_data, _plot_data, sample_rate, bipolar);

  _graph_data.clear();
  float transform = width / static_cast<float>(_plot_data.size());
  for (std::size_t i = 0; i < _plot_data.size(); i++)
    _graph_data.push_back({ static_cast<float>(i * transform), _plot_data[i] * height});
  return _graph_data;
}

template class graph_processor<float>;
template class graph_processor<cv_sample>;
template class graph_processor<audio_sample32>;

} // namespace svn::base