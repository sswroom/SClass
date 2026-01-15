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
		typedef void (CALLBACKFUNC ScrollEvent)(AnyType userObj, UIntOS scrollPos);

	private:
		Data::ArrayListObj<Data::CallbackStorage<ScrollEvent>> scrollHandlers;

	public:
		GUITrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUITrackBar();

		virtual Text::CStringNN GetObjectClass() const;
		void EventScrolled(UIntOS scrollPos);
		void HandleScrolled(ScrollEvent hdlr, AnyType userObj);

		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
		virtual void SetPos(UIntOS pos) = 0;
		virtual void SetRange(UIntOS minVal, UIntOS maxVal) = 0;
		virtual UIntOS GetPos() = 0;

	};
}
#endif
