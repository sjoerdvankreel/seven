#ifndef SVN_BASE_DSP_IO_TYPES_HPP
#define SVN_BASE_DSP_IO_TYPES_HPP

#include <svn.base/support/support.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <cstdint>

namespace svn::base {

struct note_event
{
  // Note on/off.
  bool note_on = false;
  // Velocity in case of note on.
  float velocity = 0.0f;
  // Midi note index for both on and off events.
  std::int32_t midi = -1;
  // Index in current buffer.
  std::int32_t sample_index = -1;
};

// Audio processor input.
struct block_input
{
  // Block tempo. Vst3 doesnt handle this per-sample.
  float bpm;

  // Midi note events.
  note_event* notes;
  std::int32_t note_count;

  // Sample based stream stuff.
  std::int32_t sample_count;
  std::int64_t stream_position;
  param_value* const* automation;
};

// Audio processor result.
struct block_output
{
  audio_sample* audio;
  param_value* output_params;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_IO_TYPES_HPP