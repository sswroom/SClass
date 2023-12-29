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
		GUITrackBar(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUITrackBar();

		virtual Text::CStringNN GetObjectClass() const;
		void EventScrolled(UOSInt scrollPos);
		void HandleScrolled(ScrollEvent hdlr, void *userObj);

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void SetPos(UOSInt pos) = 0;
		virtual void SetRange(UOSInt minVal, UOSInt maxVal) = 0;
		virtual UOSInt GetPos() = 0;

	};
}
#endif
