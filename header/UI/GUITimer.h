#ifndef _SM_UI_GUITIMER
#define _SM_UI_GUITIMER
#include "AnyType.h"
#include "UI/GUIForm.h"

namespace UI
{
	class GUITimer
	{
	private:
		UI::UIEvent handler;
		AnyType userObj;

	public:
		GUITimer(UI::UIEvent handler, AnyType userObj);
		virtual ~GUITimer();

		void EventTick();
	};
}
#endif
