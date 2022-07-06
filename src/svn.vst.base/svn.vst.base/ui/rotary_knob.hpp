#ifndef SVN_VST_BASE_UI_ROTARY_KNOB_HPP
#define SVN_VST_BASE_UI_ROTARY_KNOB_HPP

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/viewcreator/knobcreator.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace svn::vst::base {

// Custom drawn knob, follows color palette.
class rotary_knob : 
public VSTGUI::CKnobBase
{
  VSTGUI::CColor const _color;
public:
  CLASS_METHODS(rotary_knob, CKnobBase)
  void draw(VSTGUI::CDrawContext* context) override;
public:
  explicit rotary_knob(VSTGUI::CColor const& color):
  CKnobBase(VSTGUI::CRect(0, 0, 0, 0), nullptr, -1, nullptr), _color(color) {}
};

// VSTGUI knob factory.
class rotary_knob_creator :
public VSTGUI::UIViewCreator::KnobBaseCreator
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "seven_rotary_knob"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCControl; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_ROTARY_KNOB_HPP