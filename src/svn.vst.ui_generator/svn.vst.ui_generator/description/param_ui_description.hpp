#ifndef SVN_VST_UI_GENERATOR_DESCRIPTION_PARAM_UI_DESCRIPTION
#define SVN_VST_UI_GENERATOR_DESCRIPTION_PARAM_UI_DESCRIPTION

#include <cstdint>

namespace svn::vst::ui_generator {

struct param_ui_description
{
  std::int32_t row;
  std::int32_t column;
  std::int32_t runtime_param_index;
};

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_DESCRIPTION_PARAM_UI_DESCRIPTION