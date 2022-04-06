#ifndef _SM_UI_GUITRACKBAR
#define _SM_UI_GUITRACKBAR
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITrackBar : public GUIControl
	{
	public:
		typedef void (__stdcall *ScrollEvent)(void *userObj, UOSInt scrollPos);

	private:
		Data::ArrayList<ScrollEvent> scrollHandlers;
		Data::ArrayList<void *> scrollHandlersObj;

	public:
		void EventScrolled();
	public:
		GUITrackBar(GUICore *ui, UI::GUIClientControl *parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal);
		virtual ~GUITrackBar();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		void SetPos(UOSInt pos);
		void SetRange(UOSInt minVal, UOSInt maxVal);
		UOSInt GetPos();

		void HandleScrolled(ScrollEvent hdlr, void *userObj);
	};
}
#endif
