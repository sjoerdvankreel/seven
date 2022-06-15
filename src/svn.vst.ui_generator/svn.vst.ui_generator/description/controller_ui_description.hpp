#ifndef SVN_VST_UI_GENERATOR_DESCRIPTION_CONTROLLER_UI_DESCRIPTION
#define SVN_VST_UI_GENERATOR_DESCRIPTION_CONTROLLER_UI_DESCRIPTION

#include <svn.base/topology/topology_info.hpp>
#include <svn.vst.ui_generator/description/part_ui_description.hpp>

#include <vector>
#include <cstdint>
#include <ostream>

namespace svn::vst::ui_generator {

struct controller_ui_description
{
  std::int32_t width;
  std::int32_t height;
  std::vector<part_ui_description> parts;
  std::vector<std::int32_t> column_widths;

  static controller_ui_description
  create(svn::base::topology_info const& topology);
  void print(svn::base::topology_info const& topology, std::ostream& os);
};

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_DESCRIPTION_CONTROLLER_UI_DESCRIPTION