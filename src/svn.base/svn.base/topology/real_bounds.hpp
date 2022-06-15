#ifndef SVN_BASE_TOPOLOGY_REAL_BOUNDS_HPP
#define SVN_BASE_TOPOLOGY_REAL_BOUNDS_HPP

#include <cmath>
#include <limits>
#include <cassert>

namespace svn::base {

struct real_slope_t { enum value { linear, logarithmic, decibel, count }; };
typedef real_slope_t::value real_slope;

// Dsp/display bounds for real valued params.
struct real_bounds
{
  float const min; // Minimum transformed value for real parameters.
  float const max; // Maximum transformed value for real parameters.
  float const exp; // Slope value for real logarithmic parameters.
  real_slope const slope; // Slope type.

  float to_range(float val) const;
  float from_range(float val) const;

  static inline real_bounds unit() 
  { return { 0.0, 1.0, 0.0, real_slope::linear }; }
  static inline real_bounds linear(float min, float max) 
  { return { min, max, 0.0, real_slope::linear }; }
  static inline real_bounds decibel() 
  { return { -std::numeric_limits<float>::infinity(), 0.0, 0.0, real_slope::decibel }; }
  static inline real_bounds log(float min, float max, float ref) 
  { return { min, max, std::log((ref - min) / (max - min)) / std::log(0.5f), real_slope::linear }; }
};


inline float
real_bounds::to_range(float val) const
{
  switch (slope)
  {
  case real_slope::linear: return min + (max - min) * val;
  case real_slope::decibel: return 20.0f * std::log10(val);
  case real_slope::logarithmic: return std::pow(val, exp) * (max - min) + min;
  default: assert(false); return 0.0f;
  }
}

inline float
real_bounds::from_range(float val) const
{
  switch (slope)
  {
  case real_slope::linear: return (val - min) / (max - min);
  case real_slope::decibel: return std::exp(std::log(10.0f) * val / 20.0f);
  case real_slope::logarithmic: return std::pow((val - min) / (max - min), 1.0f / exp);
  default: assert(false); return 0.0f;
  }
}

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_REAL_BOUNDS_HPP
