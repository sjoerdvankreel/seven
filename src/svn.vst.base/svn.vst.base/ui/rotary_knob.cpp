#include <svn.vst.base/ui/support.hpp>
#include <svn.vst.base/ui/rotary_knob.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

#include <cassert>
#include <numbers>

using namespace VSTGUI;

namespace svn::vst::base {

CView* 
rotary_knob_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ 
  bool ok;
  CColor color;
  std::string const* color_name = attrs.getAttributeValue("color");
  assert(color_name != nullptr);
  ok = UIViewCreator::stringToColor(color_name, color, desc);
  assert(ok);
  return new rotary_knob(color);
} 

void
rotary_knob::draw(VSTGUI::CDrawContext* context)
{
  auto pos = getViewSize().getTopLeft();
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));

  // correct
  auto size = getViewSize().getSize();
  if (static_cast<std::int32_t>(std::ceil(size.x)) % 2 == 0) size.x -= 1.0f;
  if (static_cast<std::int32_t>(std::ceil(size.y)) % 2 == 0) size.y -= 1.0f;

  // fill
  context->setDrawMode(kAntiAliasing);
  context->setFillColor(CColor(_color.red, _color.green, _color.blue, 128));
  context->drawEllipse(CRect(CPoint(1, 1), size - CPoint(2, 2)), kDrawFilled);

  // outer border
  context->setFrameColor(color_darken(_color, 0.5f, 255));
  context->drawEllipse(CRect(CPoint(1, 1), size - CPoint(2, 2)), kDrawStroked);

  // inner border
  context->setFrameColor(color_lighten(_color, 0.25f, 255));
  context->drawEllipse(CRect(CPoint(2, 2), size - CPoint(4, 4)), kDrawStroked);
   
  // marker
  float center = (size.x - 1.0f) / 2.0f;
  float radius = center - 3.0f;
  float start = getStartAngle();
  float range = getRangeAngle();
  float angle = getValueNormalized();
  float theta = -(start + angle * range);
  float x = radius * std::sin(theta) + center;
  float y = radius * std::cos(theta) + center;
  context->setFrameColor(color_lighten(_color, 0.5f, 192));
  context->drawLine(CPoint(center, center), CPoint(x, y));

  // light settings
  std::int32_t light_parts = 15;
  std::int32_t light_top_index = light_parts / 2;

  // highlight
  std::int32_t highlight_max_alpha = 192;
  std::int32_t highlight_part_alpha_contrib = highlight_max_alpha / ((light_parts + 1) / 2);
  for (std::int32_t i = 0; i < light_parts; i++)
  {
    std::int32_t alpha_index = i <= light_top_index ? i: light_top_index - (i - light_top_index);
    float angle1 = 180.0f + i * 90.0f / light_parts;
    float angle2 = 180.0f + (i + 1) * 90.0f / light_parts;
    context->setFrameColor(CColor(255, 255, 255, (alpha_index + 1) * highlight_part_alpha_contrib));
    context->drawArc(CRect(CPoint(1, 1), size - CPoint(2, 2)), angle1, angle2, kDrawStroked);
  }

  // shadow
  std::int32_t shadow_max_alpha = 255;
  std::int32_t shadow_part_alpha_contrib = shadow_max_alpha / ((light_parts + 1) / 2);
  for (std::int32_t i = 0; i < light_parts; i++)
  {
    std::int32_t alpha_index = i <= light_top_index ? i : light_top_index - (i - light_top_index);
    float angle1 = 0.0f + i * 90.0f / light_parts;
    float angle2 = 0.0f + (i + 1) * 90.0f / light_parts;
    context->setFrameColor(CColor(0, 0, 0, (alpha_index + 1) * shadow_part_alpha_contrib));
    context->drawArc(CRect(CPoint(1, 1), size - CPoint(2, 2)), angle1, angle2, kDrawStroked);
  }
} 

} // namespace svn::vst::base