#ifndef SVN_BASE_DSP_SPECTRUM_ANALYZER_HPP
#define SVN_BASE_DSP_SPECTRUM_ANALYZER_HPP

#include <vector>
#include <complex>
#include <cstdint>

namespace svn::base {

class spectrum_analyzer 
{
  std::vector<float> _output;
  std::vector<std::complex<float>> _fft_inout;
  std::vector<std::complex<float>> _fft_scratch;

public:
  static inline std::int32_t const bucket_count = 144;

public:
  spectrum_analyzer() : _output(), _fft_inout(), _fft_scratch() {}
  // Returns spectral power at midi notes [0 .. bucket_count) in range [0..1].
  float const* analyze(float* audio, std::size_t count, float sample_rate);

private:
  float power_at_note(std::int32_t midi, float sample_rate) const;
  void fft(std::complex<float>* inout, std::complex<float>* scratch, std::size_t count);
};

} // namespace svn::base
#endif // SVN_BASE_DSP_SPECTRUM_ANALYZER_HPP