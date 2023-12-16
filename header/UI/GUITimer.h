#ifndef _SM_UI_GUITIMER
#define _SM_UI_GUITIMER
#include "UI/GUIForm.h"

namespace UI
{
	class GUITimer
	{
	private:
		UI::UIEvent handler;
		void *userObj;

	public:
		GUITimer(UI::UIEvent handler, void *userObj);
		virtual ~GUITimer();

		void EventTick();
	};
}
#endif
