#include <svn.base/static/param_bounds.hpp>

namespace svn::base {

param_bounds::
param_bounds(param_slope slope, float min, float max, float exp) :
slope(slope), min(min), max(max), exp(exp)
{
  assert(min < max);
  assert(slope == param_slope::logarithmic || exp == 0.0f);
}

param_bounds 
param_bounds::linear(float min, float max)
{ return { param_slope::linear, min, max, 0.0f }; }

param_bounds 
param_bounds::none()
{ return { param_slope::linear, 0.0f, 0.0f, 0.0f }; }

param_bounds
param_bounds::decibel()
{
  float inf = std::numeric_limits<float>::infinity();
  return { param_slope::decibel, -inf, 0.0f, 0.0f };
}

param_bounds 
param_bounds::logarithmic(float min, float max, float reference)
{ 
  float exp = std::log((reference - min) / (max - min)) / std::log(0.5f);
  return { param_slope::logarithmic, min, max, exp }; 
}

} // namespace svn::base
