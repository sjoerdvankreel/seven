#ifndef SVN_VST_BASE_UI_SUPPORT_HPP
#define SVN_VST_BASE_UI_SUPPORT_HPP

#include <vstgui/lib/ccolor.h>
#include <cstdint>

namespace svn::vst::base {

VSTGUI::CColor 
color_darken(VSTGUI::CColor const& color, float amt, std::uint8_t new_alpha);
VSTGUI::CColor
color_lighten(VSTGUI::CColor const& color, float amt, std::uint8_t new_alpha);

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_SUPPORT_HPP