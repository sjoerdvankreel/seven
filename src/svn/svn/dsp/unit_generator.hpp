#ifndef SVN_DSP_UNIT_GENERATOR_HPP
#define SVN_DSP_UNIT_GENERATOR_HPP

#include <svn/support/topo_static.hpp>
#include <svn/support/param_value.hpp>
#include <svn/support/audio_sample.hpp>
#include <svn/support/input_buffer.hpp>

namespace svn {

class unit_generator
{
  float _phase = 0.0f;
public:
  void process_block(input_buffer const& input, audio_sample* audio, param_value* state);
};

} // namespace svn
#endif // SVN_DSP_UNIT_GENERATOR_HPP