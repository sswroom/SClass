#ifndef _SM_UI_GUIVSPLITTER
#define _SM_UI_GUIVSPLITTER
#include "UI/GUIControl.h"

namespace UI
{
	class GUIVSplitter : public GUIControl
	{
	public:
		GUIVSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIVSplitter();

		virtual Text::CStringNN GetObjectClass() const;
		virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos) = 0;
		virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos) = 0;
	};
}
#endif
