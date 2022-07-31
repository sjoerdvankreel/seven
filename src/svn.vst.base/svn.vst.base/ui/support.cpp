#include <svn.vst.base/ui/support.hpp>

using namespace VSTGUI;

namespace svn::vst::base {

CColor 
color_darken(CColor const& color, float amt, std::uint8_t new_alpha)
{
  std::uint8_t r = static_cast<std::uint8_t>(color.red * (1.0f - amt));
  std::uint8_t g = static_cast<std::uint8_t>(color.green * (1.0f - amt));
  std::uint8_t b = static_cast<std::uint8_t>(color.blue * (1.0f - amt));
  return CColor(r, g, b, new_alpha);
}

CColor
color_lighten(CColor const& color, float amt, std::uint8_t new_alpha)
{
  float r = color.red / 255.0f;
  float g = color.green / 255.0f;
  float b = color.blue / 255.0f;
  std::uint8_t rn = static_cast<std::uint8_t>((r + (1.0 - r) * amt) * 255.0f);
  std::uint8_t gn = static_cast<std::uint8_t>((g + (1.0 - g) * amt) * 255.0f);
  std::uint8_t bn = static_cast<std::uint8_t>((b + (1.0 - b) * amt) * 255.0f);
  return CColor(rn, gn, bn, new_alpha);
}

} // namespace svn::vst::base