#include <svn.base/dsp/automation_buffer.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

namespace svn::base {

void* const*
automation_buffer::data()
{ return _buffer.data(); }

automation_buffer::
automation_buffer(
  struct runtime_topology const* topology,
  std::int32_t max_sample_count):
_buffer(topology->params.size()),
_real(topology->real_count * max_sample_count),
_discrete(topology->discrete_count * max_sample_count)
{
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  std::int32_t real_p = 0;
  std::int32_t discrete_p = 0;
  for (std::size_t p = 0; p < topology->params.size(); p++)
    switch (topology->params[p].descriptor->type)
    {
    case param_type::real: _buffer[p] = _real.data() + real_p++ * max_sample_count; break;
    default: _buffer[p] = _discrete.data() + discrete_p++ * max_sample_count; break;
    }
}

} // namespace svn::base