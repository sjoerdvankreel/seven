#ifndef SVN_BASE_DSP_AUTOMATION_BUFFER_HPP
#define SVN_BASE_DSP_AUTOMATION_BUFFER_HPP

#include <vector>
#include <cstdint>

namespace svn::base {

class automation_buffer
{
  // Float* for real, std::int32_t* for discrete. 
  // Of size total runtime parameter count in topology.
  std::vector<void*> _buffer;
  // Contiguous float array of size total real parameters in topology * max_sample_count.
  std::vector<float> _real;
  // Contiguous std::int32_t array of size total discrete parameters in topology * max_sample_count.
  std::vector<std::int32_t> _discrete;

public:
  void* const* data();
  automation_buffer(
    struct runtime_topology const* topology,
    std::int32_t max_sample_count);
};

} // namespace svn::base
#endif // SVN_BASE_DSP_AUTOMATION_BUFFER_HPP