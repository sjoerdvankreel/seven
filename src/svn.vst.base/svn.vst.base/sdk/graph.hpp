#ifndef SVN_VST_BASE_SDK_GRAPH_HPP
#define SVN_VST_BASE_SDK_GRAPH_HPP

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <svn.base/topology/graph_descriptor.hpp>

namespace svn::vst::base {

// Graph component backed by graph_descriptor.
class graph : 
public VSTGUI::CView
{
public:
  graph(VSTGUI::CRect const& size): VSTGUI::CView(size) { }
};

// VSTGUI graph factory.
class graph_factory : 
public VSTGUI::ViewCreatorAdapter
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "seven_graph"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCView; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const&, const VSTGUI::IUIDescription*) const override
  { return new graph(VSTGUI::CRect(0, 0, 0, 0)); }
};

} // namespace Svn::Vst
#endif // SVN_VST_BASE_SDK_GRAPH_HPP