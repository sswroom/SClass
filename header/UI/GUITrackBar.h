#ifndef _SM_UI_GUITRACKBAR
#define _SM_UI_GUITRACKBAR
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITrackBar : public GUIControl
	{
	public:
		typedef void (CALLBACKFUNC ScrollEvent)(AnyType userObj, UOSInt scrollPos);

	private:
		Data::ArrayList<Data::CallbackStorage<ScrollEvent>> scrollHandlers;

	public:
		GUITrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUITrackBar();

		virtual Text::CStringNN GetObjectClass() const;
		void EventScrolled(UOSInt scrollPos);
		void HandleScrolled(ScrollEvent hdlr, AnyType userObj);

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void SetPos(UOSInt pos) = 0;
		virtual void SetRange(UOSInt minVal, UOSInt maxVal) = 0;
		virtual UOSInt GetPos() = 0;

	};
}
#endif
