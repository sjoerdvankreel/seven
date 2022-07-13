#include <svn.vst.base/ui/graph_plot.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <svn.vst.base/support/bootstrap.hpp>
#include <svn.base/dsp/graph_processor.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

#include <cassert>
#include <cstdint>

using namespace VSTGUI;
using namespace svn::base;

namespace svn::vst::base { 

CView* 
graph_plot_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ 
  bool ok;
  CColor color;
  std::int32_t part_type;
  std::int32_t graph_type;
  std::int32_t part_index;
  std::string const* tooltip;

  std::string const* color_name = attrs.getAttributeValue("color");
  assert(color_name != nullptr);
  ok = UIViewCreator::stringToColor(color_name, color, desc);
  assert(ok);
  ok = attrs.getIntegerAttribute("part-type", part_type);
  assert(ok);
  ok = attrs.getIntegerAttribute("graph-type", graph_type);
  assert(ok);
  ok = attrs.getIntegerAttribute("part-index", part_index);
  assert(ok);
  tooltip = attrs.getAttributeValue("tooltip");
  graph_processor_base* processor = svn_create_graph_processor(svn_vst_get_topology(), part_type, graph_type, part_index);
  assert(processor != nullptr);
  auto result = new graph_plot(color, processor); 
  if(tooltip != nullptr) result->setTooltipText(tooltip->data());
  return result;
} 

void
graph_plot::draw(VSTGUI::CDrawContext* context)
{
  std::int32_t const padx = 2;
  std::int32_t const pady = 4;

  auto size = getViewSize().getSize();
  auto pos = getViewSize().getTopLeft();
  CPoint inner_size = size - CPoint(2, 2);
  CPoint render_size(size.x - padx * 2, size.y - pady * 2);
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));
  
  CColor background(0, 0, 0, 128);
  context->setFillColor(background);
  context->drawRect(CRect(CPoint(1, 1), inner_size), CDrawStyle::kDrawFilled);

  CColor border(255, 255, 255, 64);
  context->setFrameColor(border);
  context->drawRect(CRect(CPoint(1, 1), inner_size), CDrawStyle::kDrawStroked);

  context->setFrameColor(_color);
  context->setDrawMode(kAntiAliasing);
  CGraphicsPath* path = context->createGraphicsPath();
  auto editor = static_cast<VST3Editor*>(getFrame()->getEditor());
  auto state = static_cast<controller const*>(editor->getController())->state();
  std::vector<graph_point> const& graph_data = _processor->plot(state, sample_rate, bpm, render_size.x, render_size.y);
  path->beginSubpath(CPoint(graph_data[0].x + padx, render_size.y - graph_data[0].y + pady));
  for(std::size_t i = 1; i < graph_data.size(); i++)
    path->addLine(graph_data[i].x + padx, render_size.y - graph_data[i].y + pady);
  context->drawGraphicsPath(path, CDrawContext::kPathStroked);
  path->forget();
}   

} // namespace svn::vst::base