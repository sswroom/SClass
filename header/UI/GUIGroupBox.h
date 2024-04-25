#ifndef _SM_UI_GUIGROUPBOX
#define _SM_UI_GUIGROUPBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIGroupBox : public GUIClientControl
	{
	public:
		GUIGroupBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIGroupBox();

		virtual Math::Coord2DDbl GetClientOfst() = 0;
		virtual Math::Size2DDbl GetClientSize() = 0;

		virtual Bool IsChildVisible();
		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
	};
}
#endif
