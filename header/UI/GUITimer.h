#ifndef _SM_UI_GUITIMER
#define _SM_UI_GUITIMER
#include "UI/GUIForm.h"

namespace UI
{
	class GUITimer
	{
	private:
		NotNullPtr<UI::GUIForm> parent;
		UOSInt id;
		UInt32 interval;
		UI::UIEvent handler;
		void *userObj;

	public:
		GUITimer(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIForm> parent, UOSInt id, UInt32 interval, UI::UIEvent handler, void *userObj);
		virtual ~GUITimer();

		virtual void OnTick();
		UOSInt GetId();
	};
}
#endif
