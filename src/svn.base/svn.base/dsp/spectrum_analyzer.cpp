#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/spectrum_analyzer.hpp>

#include <numbers>
#include <cassert>

namespace svn::base {

// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void
spectrum_analyzer::fft(std::complex<float>* inout, 
  std::complex<float>* scratch, std::size_t count)
{
  if (count < 2) return;
  assert(count == next_pow2(count));
  std::complex<float>* even = scratch;
  std::complex<float>* odd = scratch + count / 2;
  for (std::size_t i = 0; i < count / 2; i++) even[i] = inout[i * 2];
  for (std::size_t i = 0; i < count / 2; i++) odd[i] = inout[i * 2 + 1];
  fft(odd, inout, count / 2);
  fft(even, inout, count / 2);
  for (std::size_t i = 0; i < count / 2; i++)
  {
    float im = -2.0f * std::numbers::pi * i / count;
    std::complex<float> t = std::polar(1.0f, im) * odd[i];
    inout[i] = even[i] + t;
    inout[i + count / 2] = even[i] - t;
  }
}

// https://stackoverflow.com/questions/604453/analyze-audio-using-fast-fourier-transform
// https://dsp.stackexchange.com/questions/46692/calculating-1-3-octave-spectrum-from-fft-dft
float 
spectrum_analyzer::power_at_note(std::int32_t midi, float sample_rate) const
{
  float result = 0.0f;
  float freq_to_bin = sample_rate / (_fft_inout.size() * 2.0f);
  std::size_t bin1 = static_cast<size_t>(note_to_frequency(midi) * freq_to_bin);
  std::size_t bin2 = static_cast<size_t>(note_to_frequency(midi + 1) * freq_to_bin);
  for (std::size_t i = bin1; i < bin2 && i < _fft_inout.size(); i++)
  {
    float real2 = _fft_inout[i].real() * _fft_inout[i].real();
    float imag2 = _fft_inout[i].imag() * _fft_inout[i].imag();
    result += real2 + imag2;
  }
  return sanity(std::sqrtf(result));
}

float const*
spectrum_analyzer::analyze(float* audio, std::size_t count, float sample_rate)
{
  _fft_inout.clear();
  std::size_t pow2 = next_pow2(count);
  for (std::size_t i = 0; i < count; i++)
    _fft_inout.push_back(std::complex<float>(audio[i], 0.0f));
  _fft_inout.insert(_fft_inout.end(), pow2 - count, std::complex<float>(0.0f, 0.0f));
  _fft_scratch.resize(pow2);
  fft(_fft_inout.data(), _fft_scratch.data(), pow2);
  // Discard above nyquist.
  _fft_inout.erase(_fft_inout.begin() + _fft_inout.size() / 2, _fft_inout.end());

  _output.clear();
  float max = 0.0f;
  for (std::int32_t oct = 0; oct < 12; oct++)
    for (std::int32_t note = 0; note < 12; note++)
      _output.push_back(power_at_note(oct * 12 + note, sample_rate));
  for (std::size_t i = 0; i < _output.size(); i++) max = std::max(_output[i], max);
  for (std::size_t i = 0; i < _output.size(); i++) _output[i] = max == 0.0f ? 0.0f : sanity_unipolar(_output[i] / max);
  return _output.data();
}

} // namespace svn::base