#include <svn.vst.base/sdk/graph.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

#include <cassert>
#include <cstdint>

using namespace VSTGUI;

namespace svn::vst::base { 

CView* 
graph_creator::create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const
{ 
  CColor color;
  std::string const* color_name = attrs.getAttributeValue("color");
  bool ok = UIViewCreator::stringToColor(color_name, color, desc);
  assert(ok);
  return new graph(VSTGUI::CRect(0, 0, 0, 0), color); 
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

  context->setFrameColor(_color);
  context->setDrawMode(kAntiAliasing);
  context->drawLine(inner_pos, inner_pos + inner_size);
}

} // namespace svn::vst::base