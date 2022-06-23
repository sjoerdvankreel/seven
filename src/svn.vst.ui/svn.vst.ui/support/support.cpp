#include <svn.vst.ui/support/support.hpp>

#include <cassert>
#include <stdexcept>

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

} // namespace svn::vst::ui