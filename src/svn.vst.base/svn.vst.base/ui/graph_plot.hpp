#ifndef SVN_VST_BASE_UI_GRAPH_PLOT_HPP
#define SVN_VST_BASE_UI_GRAPH_PLOT_HPP

#include <svn.base/dsp/graph_processor.hpp>
#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace svn::vst::base {

// Graph component backed by graph_descriptor and graph_processor.
class graph_plot : 
public VSTGUI::CView
{
  static inline float const bpm = 120.0f;
  static inline float const sample_rate = 48000.0f;

  VSTGUI::CColor const _color;
  std::int32_t const _row_span;
  std::int32_t const _column_span;
  svn::base::graph_processor_base* const _processor;
public:
  void draw(VSTGUI::CDrawContext* context) override;
  graph_plot(VSTGUI::CColor const& color, std::int32_t row_span,
    std::int32_t column_span, svn::base::graph_processor_base* processor):
  VSTGUI::CView(VSTGUI::CRect(0, 0, 0, 0)), _color(color), 
  _row_span(row_span), _column_span(column_span), _processor(processor) { }
public:
  ~graph_plot() override { delete _processor; }
  svn::base::graph_processor_base* processor() const { return _processor; }
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

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_GRAPH_PLOT_HPP