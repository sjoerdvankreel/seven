#include <svn.base/dsp/graph_processor.hpp>
#include <cmath>

namespace svn::base {

std::vector<graph_point> const&
graph_processor::process(param_value const* state, 
  std::int32_t width, std::int32_t height, float sample_rate)
{
  _automation.clear();
  _automation_buffer.clear();
  std::int32_t samples = sample_count(state, sample_rate);
  for(std::int32_t p = 0; p < topology()->input_param_count; p++)
    for(std::int32_t s = 0; s < samples; s++)
      _automation_buffer.push_back(state[p]);
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    _automation.push_back(_automation_buffer.data() + static_cast<std::size_t>(samples));

  block_input input;
  input.bpm = 0.0f;
  input.note_count = 0;
  input.notes = nullptr;
  input.stream_position = 0;
  input.sample_count = samples;
  input.automation = _automation.data();

  block_output output;
  _audio_data.clear();
  _audio_data.resize(samples);
  output.audio = _audio_data.data();
  output.output_params = _output_param_buffer.data();
  process_audio(input, output, sample_rate);

  _graph_data.clear();
  for (std::int32_t i = 0; i < width; i++)
  {
    float x = i * _audio_data.size() / static_cast<float>(width);
    std::int32_t x0_pos = static_cast<std::int32_t>(std::floor(x));
    std::int32_t x1_pos = static_cast<std::int32_t>(std::ceil(x));
    float x0_weight = 1.0f - (x - x0_pos);
    float x1_weight = x1_pos - x;
    audio_sample stereo = _audio_data[x0_pos] * x0_weight + _audio_data[x1_pos] + x1_weight;
    float y = (stereo.left + stereo.right + 1.0f) * 0.5f * height;
    _graph_data.push_back({x, y});
  }
  return _graph_data;
}

} // namespace svn::base