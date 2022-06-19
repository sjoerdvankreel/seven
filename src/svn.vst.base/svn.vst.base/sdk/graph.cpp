#include <svn.vst.base/sdk/graph.hpp>
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
graph_creator::create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const
{ 
  bool ok;
  CColor color;
  std::int32_t part_type;
  std::int32_t part_index;

  std::string const* color_name = attrs.getAttributeValue("color");
  assert(color_name != nullptr);
  ok = UIViewCreator::stringToColor(color_name, color, desc);
  assert(ok);
  ok = attrs.getIntegerAttribute("part-type", part_type);
  assert(ok);
  ok = attrs.getIntegerAttribute("part-index", part_index);
  assert(ok);
  graph_processor* processor = svn_create_graph_processor(svn_vst_get_topology(), part_type, part_index);
  assert(processor != nullptr);
  return new graph(VSTGUI::CRect(0, 0, 0, 0), color, processor); 
}

void
graph::draw(VSTGUI::CDrawContext* context)
{
  auto size = getViewSize().getSize();
  auto pos = getViewSize().getTopLeft();
  CPoint inner_size = size - CPoint(2, 2);
  CPoint render_size(size.x - 2, size.y - 5);
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));
  
  CColor background(0, 0, 0, 128);
  context->setFillColor(background);
  context->drawRect(CRect(CPoint(1, 1), inner_size), CDrawStyle::kDrawFilled);

  CColor border(255, 255, 255, 64);
  context->setFrameColor(border);
  context->drawRect(CRect(CPoint(1, 1), inner_size), CDrawStyle::kDrawStroked);

  CDrawContext::LineList list;
  context->setFrameColor(_color);
  context->setDrawMode(kAntiAliasing);
  auto editor = static_cast<VST3Editor*>(getFrame()->getEditor());
  auto state = static_cast<controller const*>(editor->getController())->state();
  std::vector<graph_point> const& graph_data = _processor->process(state, render_size.x, render_size.y, 48000.0);
  for(std::size_t i = 1; i < graph_data.size(); i++)
  {
    CPoint first = CPoint(graph_data[i - 1].x + 2, render_size.y - graph_data[i - 1].y + 2);
    CPoint second = CPoint(graph_data[i].x + 2, render_size.y - graph_data[i].y + 2);
    list.push_back(CDrawContext::LinePair(first, second));
  }  
  context->drawLines(list);
}

} // namespace svn::vst::base