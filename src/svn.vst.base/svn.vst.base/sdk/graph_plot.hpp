#ifndef SVN_VST_BASE_SDK_GRAPH_PLOT_HPP
#define SVN_VST_BASE_SDK_GRAPH_PLOT_HPP

#include <svn.base/dsp/graph_processor.hpp>
#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace svn::vst::base {

// Graph component backed by graph_descriptor and graph_processor.
class graph_plot : 
public VSTGUI::CView
{
  VSTGUI::CColor const _color;
  svn::base::graph_processor* const _processor;
public:
  ~graph_plot() override { delete _processor; }
  svn::base::graph_processor* const processor() const { return _processor; }
public:
  void draw(VSTGUI::CDrawContext* context) override;
  graph_plot(VSTGUI::CRect const& size, VSTGUI::CColor const& color, svn::base::graph_processor* processor):
  VSTGUI::CView(size), _color(color), _processor(processor) { }
};

// VSTGUI graph factory.
class graph_plot_creator :
public VSTGUI::ViewCreatorAdapter
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "seven_graph_plot"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCView; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace Svn::Vst
#endif // SVN_VST_BASE_SDK_GRAPH_PLOT_HPP