#include <svn.vst.base/sdk/graph.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

#include <cassert>
#include <cstdint>

using namespace VSTGUI;

static std::int32_t const padding_param_group = 1;

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
  auto pos = getViewSize().getTopLeft();
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));
  context->setFrameColor(_color);
  context->drawLine(CPoint(0, 0), CPoint(10, 10));
}

} // namespace svn::vst::base