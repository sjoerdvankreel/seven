#include <svn.base/dsp/graph_processor.hpp>
#include <cmath>

namespace svn::base {

std::vector<graph_point> const&
graph_processor::process(std::int32_t width, std::int32_t height, float sample_rate)
{
  _audio_data.clear();
  process_audio(_audio_data, sample_rate);
  _graph_data.clear();
  for (std::int32_t i = 0; i < width; i++)
  {
    float x = i * _audio_data.size() / static_cast<float>(width);
    std::int32_t x0_pos = static_cast<std::int32_t>(std::floor(x));
    std::int32_t x1_pos = static_cast<std::int32_t>(std::ceil(x));
    float x0_weight = 1.0f - (x - x0_pos);
    float x1_weight = x1_pos - x;
    float sample = _audio_data[x0_pos] * x0_weight + _audio_data[x1_pos] + x1_weight;
    float y = (sample + 1.0f) * 0.5f * height;
    _graph_data.push_back({x, y});
  }
  return _graph_data;
}

} // namespace svn::base