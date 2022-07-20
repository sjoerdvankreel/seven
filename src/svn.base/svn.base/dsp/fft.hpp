#ifndef SVN_BASE_DSP_FFT_HPP
#define SVN_BASE_DSP_FFT_HPP

#include <vector>
#include <complex>
#include <cstdint>

namespace svn::base {

class fft
{
  std::vector<std::complex<float>> _fft;
  std::vector<std::complex<float>> _scratch;
private:
  void transform(std::complex<float>* inout,
    std::complex<float>* scratch, std::size_t count);
public:
  fft(): _fft(), _scratch() {}
  std::vector<std::complex<float>> const& transform(
    float const* audio, std::size_t count);
};

class spectrum_analyzer 
{
  fft _fft;
  std::vector<float> _output;
public:
  static inline std::int32_t const bucket_count = 144;
private:
  float power_at_note(std::vector<std::complex<float>> const& fft, 
    std::int32_t midi, float sample_rate) const;
public:
  spectrum_analyzer() : _fft(), _output() {}
  // Returns spectral power at midi notes [0 .. bucket_count) in range [0..1].
  float const* analyze(float* audio, std::size_t count, float sample_rate);
};

} // namespace svn::base
#endif // SVN_BASE_DSP_FFT_HPP