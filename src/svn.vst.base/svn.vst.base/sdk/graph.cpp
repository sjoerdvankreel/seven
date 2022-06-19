#include <svn.vst.base/sdk/graph.hpp>
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
  graph_processor* processor = create_graph_processor(svn_vst_get_topology(), part_type, part_index);
  assert(processor != nullptr);
  return new graph(VSTGUI::CRect(0, 0, 0, 0), color, processor); 
}

void
graph::draw(VSTGUI::CDrawContext* context)
{
  CPoint inner_pos(1, 1);
  auto size = getViewSize().getSize();
  auto pos = getViewSize().getTopLeft();
  CPoint inner_size(size.x - 2, size.y - 2);
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));
  
  CColor background(255, 255, 255, 64);
  context->setFillColor(background);
  context->drawRect(CRect(CPoint(0, 0), size), CDrawStyle::kDrawFilled);

  CColor border(0, 0, 0, 192);
  context->setFrameColor(border);
  context->drawRect(CRect(CPoint(0, 0), size), CDrawStyle::kDrawStroked);

  context->setDrawMode(kAntiAliasing);
  std::vector<graph_point> const& graph_data = _processor->process(inner_size.x, inner_size.y, 48000.0);
  for(std::size_t i = 0; i < graph_data.size(); i++)
    context->drawPoint(CPoint(graph_data[i].x, graph_data[i].y), _color);
}

} // namespace svn::vst::base