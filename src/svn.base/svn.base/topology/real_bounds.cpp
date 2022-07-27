#include <svn.base/topology/real_bounds.hpp>

namespace svn::base {

void
real_bounds::from_range(float* vals, std::int32_t count) const
{
  switch (slope)
  {
  case real_slope::linear: 
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = (vals[i] - min) / (max - min);
  case real_slope::quadratic: 
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = std::sqrt((vals[i] - min) / (max - min));
  case real_slope::decibel: 
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = std::exp(std::log(10.0f) * vals[i] / 20.0f);
  case real_slope::logarithmic: 
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = std::pow((vals[i] - min) / (max - min), 1.0f / exp);
  default: 
    assert(false);
    break;
  }
}

void
real_bounds::to_range(float* vals, std::int32_t count) const
{
  switch (slope)
  {
  case real_slope::linear:
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = min + (max - min) * vals[i];
    break;
  case real_slope::decibel:
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = 20.0f * std::log10(vals[i]);
    break;
  case real_slope::quadratic:
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = min + (max - min) * vals[i] * vals[i];
    break;
  case real_slope::logarithmic:
    for (std::int32_t i = 0; i < count; i++)
      vals[i] = std::pow(vals[i], exp) * (max - min) + min;
    break;
  default:
    assert(false);
    break;
  }
}

} // namespace svn::base