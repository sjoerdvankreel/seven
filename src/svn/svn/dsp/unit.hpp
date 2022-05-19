#ifndef SVN_DSP_UNIT_HPP
#define SVN_DSP_UNIT_HPP

#include <svn/support/topo_static.hpp>
#include <svn/support/event_buffer.hpp>

namespace svn {

class unit
{
  float _phase = 0.0f;
public:
  void process(input_buffer const& input, output_buffer& output);
};

} // namespace svn
#endif // SVN_DSP_UNIT_HPP