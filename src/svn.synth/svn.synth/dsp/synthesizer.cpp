#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/synthesizer.hpp>

#include <Windows.h>
#include <cassert>
#include <algorithm>

using namespace svn::base;

namespace svn::synth {

synthesizer::
synthesizer(
  base::topology_info const* topology, float sample_rate,
  std::int32_t max_sample_count, base::param_value* state):
audio_processor(topology, sample_rate, max_sample_count, state),
_cv(topology, max_sample_count), _voice_audio(max_sample_count),
_automation_fixed(static_cast<std::size_t>(synth_polyphony)),
_automation_fixed_buffer(static_cast<std::size_t>(synth_polyphony * topology->input_param_count)),
_last_automation_previous_block(static_cast<std::size_t>(topology->input_param_count)),
_voices(), _voice_states()
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  for(std::int32_t v = 0; v < synth_polyphony; v++)
    _automation_fixed[v] = _automation_fixed_buffer.data() + v * topology->input_param_count;
  // Use defaults on the first round.
  topology->init_defaults(_last_automation_previous_block.data());
}

void
synthesizer::return_voice(std::int32_t index)
{
  assert(_voice_states[index].in_use);
  _voice_states[index] = voice_state();
}

void
synthesizer::setup_voice_release(
  note_event const& note, std::int32_t sample_count)
{
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use && 
      _voice_states[v].midi == note.midi &&
      !_voice_states[v].released_previous_buffer)
    {
      _voice_states[v].release_this_buffer = true;
      _voice_states[v].release_position_buffer = note.sample_index;
    }
}

void
synthesizer::setup_voice(
  note_event const& note, std::int32_t sample_count, std::int64_t stream_position)
{
  _voices_drained = true;
  std::int32_t slot = -1;
  std::int64_t slot_position = -1;

  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (!_voice_states[v].in_use)
    {
      // Found a free slot, no need to recycle.
      slot = v;
      _voices_drained = false;
      break;
    } else
    {
      // Keep track of oldest (stream-time start position) voice.
      std::int64_t this_position = _voice_states[v].start_position_stream;
      if (slot == -1 || this_position < slot_position)
      {
        slot = v;
        slot_position = this_position;
      }
    }

  _voice_states[slot].in_use = true;
  _voice_states[slot].release_this_buffer = false;
  _voice_states[slot].released_previous_buffer = false;
  _voice_states[slot].midi = note.midi;
  // Not released this buffer by default.
  _voice_states[slot].release_position_buffer = -1;
  _voice_states[slot].start_position_buffer = note.sample_index;
  _voice_states[slot].start_position_stream = stream_position + note.sample_index;
  new(&_voices[slot]) synth_voice(topology(), sample_rate(), note.velocity, note.midi);
}

void
synthesizer::process_block(block_input const& input, block_output& output)
{
  voice_input vinput;
  vinput.bpm = input.bpm;
  cpu_usage usage = { 0.0 };
  double start_time = performance_counter();
   
  // Correct voice info for leftovers from previous round.
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (!_voice_states[v].in_use)
      _voices_drained = false;
    else
    {
      // Run along from the beginning.
      _voice_states[v].start_position_buffer = 0;
      _voice_states[v].release_this_buffer = false;
    }      

  // Set up new activation and release state.
  for(std::int32_t n = 0; n < input.note_count; n++)
    if(input.notes[n].note_on)
      setup_voice(input.notes[n], input.sample_count, input.stream_position);
    else
      setup_voice_release(input.notes[n], input.sample_count);
  
  // For output information.
  std::int32_t voice_count = 0;
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if(_voice_states[v].in_use)
      ++voice_count;

  // Process voices that are active anywhere in this buffer.
  std::memset(output.audio, 0, input.sample_count * sizeof(base::audio_sample32));
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use)
    {
      std::int32_t voice_start = _voice_states[v].start_position_buffer;
      vinput.sample_count = input.sample_count - voice_start;
      vinput.stream_position = input.stream_position + voice_start;
      vinput.automation = base::automation_view(
        topology(), _automation_fixed[v], input.automation,
        topology()->input_param_count, topology()->input_param_count, 0, 
        input.sample_count, voice_start, vinput.sample_count);

      std::int32_t release_sample = -1;
      std::int32_t voice_release = _voice_states[v].release_position_buffer;
      
      // Already released in previous buffer, fix from the beginning.
      if (_voice_states[v].released_previous_buffer)
        vinput.automation = vinput.automation.rearrange_samples(0, 0);
      
      // Releasing this buffer, do the bookkeeping.
      else if(_voice_states[v].release_this_buffer)
      {
        // Fix voice to automation values at T-1 if released at time T.
        // This fixes the case where a new note play in the current channel
        // together with an automation value, in case we want to apply
        // the automation change only to the new note.
        for (std::int32_t p = 0; p < topology()->input_param_count; p++)
          if(voice_release == 0)
            _automation_fixed[v][p] = _last_automation_previous_block[p];
          else 
            _automation_fixed[v][p] = input.automation[p][voice_release - 1];

        release_sample = voice_release - voice_start;
        assert(release_sample >= 0);
        vinput.automation = vinput.automation.rearrange_samples(voice_start, release_sample);
        _voice_states[v].released_previous_buffer = true;
      }
      // Else nothing to do, we ride along with the active automation values.     


      bool ended = _voices[v].process_block(vinput, _cv, _voice_audio, release_sample, usage);
      base::add_audio(output.audio + voice_start, _voice_audio.amplitude.data(), vinput.sample_count);
      if(ended) return_voice(v);
    } 

  // Remember last automation values in case a note is released on the 
  // next round at sample index 0. Because if a voice is released at
  // time T then we fix it to the automation values at time T-1.
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    _last_automation_previous_block[p] = input.automation[p][input.sample_count - 1];
  
  // Clip and set output info.
  bool clip = base::clip_audio(output.audio, input.sample_count);
  std::int32_t output_start = topology()->param_bounds[part_type::output][0] - topology()->input_param_count;
  output.output_params[output_start + output_param::clip].discrete = clip ? 1 : 0;
  output.output_params[output_start + output_param::voices].discrete = voice_count;
  output.output_params[output_start + output_param::drain].discrete = _voices_drained ? 1 : 0;

  // Set cpu usage. 
  usage.total = performance_counter() - start_time;
  double part_time_factor = 100.0 / usage.total;
  double total_time_factor = 100.0 * sample_rate() / input.sample_count; 
  double part_proc_time = usage.cv + usage.osc + usage.env + usage.lfo + usage.amp + usage.audio + usage.filter;
  std::int32_t cpu_start = topology()->param_bounds[part_type::cpu][0] - topology()->input_param_count;
  output.output_params[cpu_start + cpu_param::cv].discrete = static_cast<std::int32_t>(usage.cv * part_time_factor);
  output.output_params[cpu_start + cpu_param::osc].discrete = static_cast<std::int32_t>(usage.osc * part_time_factor);
  output.output_params[cpu_start + cpu_param::env].discrete = static_cast<std::int32_t>(usage.env * part_time_factor);
  output.output_params[cpu_start + cpu_param::lfo].discrete = static_cast<std::int32_t>(usage.lfo * part_time_factor);
  output.output_params[cpu_start + cpu_param::amp].discrete = static_cast<std::int32_t>(usage.amp * part_time_factor);
  output.output_params[cpu_start + cpu_param::audio].discrete = static_cast<std::int32_t>(usage.audio * part_time_factor);
  output.output_params[cpu_start + cpu_param::total].discrete = static_cast<std::int32_t>(usage.total * total_time_factor);
  output.output_params[cpu_start + cpu_param::filter].discrete = static_cast<std::int32_t>(usage.filter * part_time_factor);
  output.output_params[cpu_start + cpu_param::aux].discrete = static_cast<std::int32_t>((usage.total - part_proc_time) * part_time_factor);
}  
  
} // namespace svn::synth