#ifndef SVN_VST_UI_GENERATOR_DESCRIPTION_PART_UI_DESCRIPTION
#define SVN_VST_UI_GENERATOR_DESCRIPTION_PART_UI_DESCRIPTION

#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.vst.ui_generator/description/param_ui_description.hpp>

#include <vector>
#include <cstdint>

namespace svn::vst::ui_generator {

struct part_ui_description
{
  std::int32_t top;
  std::int32_t left;
  std::int32_t width;
  std::int32_t height;
  std::int32_t rows;
  std::int32_t column;
  std::int32_t columns;
  std::int32_t color_index;
  std::int32_t runtime_part_index;
  param_ui_description enabled_param;
  std::vector<param_ui_description> params;

  static part_ui_description create(
    svn::base::runtime_topology const& topology, 
    std::int32_t runtime_part_index);
};

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_DESCRIPTION_PART_UI_DESCRIPTION