#ifndef SVN_BASE_STATIC_PARAM_BOUNDS_HPP
#define SVN_BASE_STATIC_PARAM_BOUNDS_HPP

#include <cmath>
#include <limits>
#include <cassert>

namespace svn::base {

struct param_slope_t { enum value { linear, logarithmic, decibel, count }; };
typedef param_slope_t::value param_slope;

struct param_bounds
{
  float const min; // Minimum transformed value for real parameters.
  float const max; // Maximum transformed value for real parameters.
  float const exp; // Slope value for real logarithmic parameters.
  param_slope const slope; // Slope type for real parameters.

  float to_range(float val) const;
  float from_range(float val) const;
  param_bounds(param_slope slope, float min, float max, float exp);

  static param_bounds none();
  static param_bounds decibel();
  static param_bounds linear_unit();
  static param_bounds linear(float min, float max);
  static param_bounds logarithmic(float min, float max, float reference);
};

inline float
param_bounds::to_range(float val) const
{
  switch (slope)
  {
  case param_slope::linear: return min + (max - min) * val;
  case param_slope::decibel: return 20.0f * std::log10(val);
  case param_slope::logarithmic: return std::pow(val, exp) * (max - min) + min;
  default: assert(false); return 0.0f;
  }
}

inline float
param_bounds::from_range(float val) const
{
  switch (slope)
  {
  case param_slope::linear: return (val - min) / (max - min);
  case param_slope::decibel: return std::exp(std::log(10.0f) * val / 20.0f);
  case param_slope::logarithmic: return std::pow((val - min) / (max - min), 1.0f / exp);
  default: assert(false); return 0.0f;
  }
}

} // namespace svn::base
#endif // SVN_BASE_STATIC_PARAM_BOUNDS_HPP
