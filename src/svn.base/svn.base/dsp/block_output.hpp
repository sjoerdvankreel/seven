#ifndef SVN_BASE_DSP_BLOCK_OUTPUT_HPP
#define SVN_BASE_DSP_BLOCK_OUTPUT_HPP

#include <svn.base/support/support.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <cstdint>

namespace svn::base {

struct block_output
{
  audio_sample* audio;
  param_value* output_params;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_BLOCK_OUTPUT_HPP