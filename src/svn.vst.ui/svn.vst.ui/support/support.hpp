#ifndef SVN_VST_UI_SUPPORT_SUPPORT_HPP
#define SVN_VST_UI_SUPPORT_SUPPORT_HPP

#include <string>

namespace svn::vst::ui {

std::string
narrow_assume_ascii(std::wstring const& wide);

} // namespace svn::vst::ui
#endif // SVN_VST_UI_SUPPORT_SUPPORT_HPP