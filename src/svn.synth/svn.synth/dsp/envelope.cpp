#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/envelope.hpp>

#include <cmath>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

void 
envelope::process_block(voice_input const& input, std::int32_t index, float* cv_out)
{
}

} // namespace svn::synth