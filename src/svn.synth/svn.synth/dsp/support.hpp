#ifndef SVN_SYNTH_DSP_SUPPORT_HPP
#define SVN_SYNTH_DSP_SUPPORT_HPP

#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/automation_view.hpp>
#include <cstdint>

namespace svn::synth {

// Voice block input.
struct voice_input
{
  float bpm;
  std::int32_t sample_count;
  std::int64_t stream_position;
  base::automation_view automation;
};

inline bool cv_sync_polarity_is_synced(std::int32_t sync_polarity)
{ return sync_polarity == cv_sync_polarity::sync_unipolar || sync_polarity == cv_sync_polarity::sync_bipolar; }
inline bool cv_sync_polarity_is_unipolar(std::int32_t sync_polarity)
{ return sync_polarity == cv_sync_polarity::time_unipolar || sync_polarity == cv_sync_polarity::sync_unipolar; }

} // namespace svn::base
#endif // SVN_SYNTH_DSP_SUPPORT_HPP