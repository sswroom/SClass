#ifndef _SM_UI_GUIGROUPBOX
#define _SM_UI_GUIGROUPBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIGroupBox : public GUIClientControl
	{
	private:
		void *oriWndProc;

		static OSInt __stdcall GBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
	public:
		GUIGroupBox(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Text::CStringNN name);
		virtual ~GUIGroupBox();

		virtual Math::Coord2DDbl GetClientOfst();
		virtual Math::Size2DDbl GetClientSize();
		virtual Bool IsChildVisible();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
	};
};
#endif
