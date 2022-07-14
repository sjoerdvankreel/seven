#include <svn.vst.ui/support/support.hpp>

#include <cassert>
#include <stdexcept>

using namespace svn::base;

namespace svn::vst::ui {

std::string
narrow_assume_ascii(std::wstring const& wide)
{
  std::string result;
  for (std::size_t c = 0; c < wide.length(); c++)
  {
    if (wide[c] > 127)
    {
      assert(!"Unexpected character.");
      std::string error("Unexpected character: ");
      error += std::to_string(static_cast<int>(wide[c]));
      error += ".";
      throw std::runtime_error(error);
    }
    result += static_cast<char>(wide[c]);
  }
  return result;
}

ui_color const
ui_color_gradient(topology_info const& topology, std::int32_t static_part_index)
{
  ui_color end_color = topology.ui.end_color;
  ui_color start_color = topology.ui.start_color;
  double r_diff = end_color.r / 255.0 - start_color.r / 255.0;
  double g_diff = end_color.g / 255.0 - start_color.g / 255.0;
  double b_diff = end_color.b / 255.0 - start_color.b / 255.0;
  double factor = static_part_index / (topology.static_part_count - 1.0);
  std::uint32_t r = static_cast<std::uint32_t>((start_color.r / 255.0 + r_diff * factor) * 255);
  std::uint32_t g = static_cast<std::uint32_t>((start_color.g / 255.0 + g_diff * factor) * 255);
  std::uint32_t b = static_cast<std::uint32_t>((start_color.b / 255.0 + b_diff * factor) * 255);
  return ui_color(r, g, b);
}

} // namespace svn::vst::ui