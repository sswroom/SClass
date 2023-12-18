#ifndef _SM_UI_GUIHSPLITTER
#define _SM_UI_GUIHSPLITTER
#include "UI/GUIControl.h"

namespace UI
{
	class GUIHSplitter : public GUIControl
	{
	public:
		GUIHSplitter(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIHSplitter();

		virtual Text::CStringNN GetObjectClass() const;
		virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos) = 0;
		virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos) = 0;
	};
}
#endif
