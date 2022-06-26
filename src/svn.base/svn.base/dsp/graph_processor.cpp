#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/graph_processor.hpp>
#include <cmath>

namespace svn::base {

// Given parameter state and sample rate, setup automation and run some dsp code.
std::vector<audio_sample32> const&
graph_processor::process_audio(param_value const* state, float sample_rate)
{
  _automation.clear();
  _automation_buffer.clear();
  std::int32_t samples = audio_sample_count(state, sample_rate);
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    for (std::int32_t s = 0; s < samples; s++)
      _automation_buffer.push_back(state[p]);
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    _automation.push_back(_automation_buffer.data() + static_cast<std::size_t>(p * samples));

  _audio_data.clear();
  _audio_data.resize(samples);

  block_input input;
  input.bpm = 0.0f;
  input.note_count = 0;
  input.notes = nullptr;
  input.stream_position = 0;
  input.sample_count = samples;
  input.automation = _automation.data();

  block_output output;
  output.audio = _audio_data.data();
  output.output_params = _output_param_buffer.data();

  // This produces the graph specific audio data.
  process_audio_core(input, output, sample_rate);
  return _audio_data;
}

std::vector<graph_point> const&
graph_processor::plot(param_value const* state, 
  float sample_rate, std::int32_t width, std::int32_t height)
{
  _plot_data.clear();
  process_audio(state, sample_rate);
  audio_to_plot(_audio_data, _plot_data, sample_rate);

  _graph_data.clear();
  float transform = width / static_cast<float>(_plot_data.size());
  for (std::size_t i = 0; i < _plot_data.size(); i++)
    _graph_data.push_back({ static_cast<float>(i * transform), _plot_data[i] * height});
  return _graph_data;
}

} // namespace svn::base