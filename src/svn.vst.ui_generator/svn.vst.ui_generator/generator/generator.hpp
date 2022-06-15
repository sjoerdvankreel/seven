#ifndef SVN_VST_UI_GENERATOR_GENERATOR_GENERATOR_HPP
#define SVN_VST_UI_GENERATOR_GENERATOR_GENERATOR_HPP

#include <svn.base/topology/topology_info.hpp>
#include <svn.vst.ui_generator/description/controller_ui_description.hpp>
#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>

namespace svn::vst::ui_generator {

rapidjson::Document
build_vstgui_json(
  svn::base::topology_info const& topology,
  controller_ui_description const& description);

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_GENERATOR_GENERATOR_HPP