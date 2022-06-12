#ifndef SVN_VST_UI_GENERATOR_SUPPORT_SUPPORT_HPP
#define SVN_VST_UI_GENERATOR_SUPPORT_SUPPORT_HPP

#include <string>

namespace svn::vst::ui_generator {

std::string
narrow_assume_ascii(std::wstring const& wide);

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_SUPPORT_SUPPORT_HPP