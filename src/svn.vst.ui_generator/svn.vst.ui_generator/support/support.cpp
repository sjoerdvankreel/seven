#include <svn.vst.ui_generator/support/support.hpp>
#include <stdexcept>

namespace svn::vst::ui_generator {

std::string
narrow_assume_ascii(std::wstring const& wide)
{
  std::string result;
  for (std::size_t c = 0; c < wide.length(); c++)
  {
    if (wide[c] > 127)
    {
      std::string error("Unexpected character: ");
      error += std::to_string(static_cast<int>(wide[c]));
      error += ".";
      throw std::runtime_error(error);
    }
    result += static_cast<char>(wide[c]);
  }
  return result;
}

} // namespace svn::vst::ui_generator