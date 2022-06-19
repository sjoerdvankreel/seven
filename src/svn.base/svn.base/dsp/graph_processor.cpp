#include <svn.base/dsp/graph_processor.hpp>

namespace svn::base {

std::vector<float> const& 
graph_processor::process(std::int32_t width, std::int32_t height, float sample_rate)
{
  _audio_data.clear();
  process_audio(_audio_data, sample_rate);
  _graph_data.clear();
  return _graph_data;
}

} // namespace svn::base