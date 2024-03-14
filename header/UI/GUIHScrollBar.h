#ifndef _SM_UI_GUIHSCROLLBAR
#define _SM_UI_GUIHSCROLLBAR
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIHScrollBar : public GUIControl
	{
	public:
		typedef void (__stdcall *PosChgEvent)(AnyType userObj, UOSInt newPos);

	private:
		Data::ArrayList<Data::CallbackStorage<PosChgEvent>> posChgHdlrs;
	
	public:
		GUIHScrollBar(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIHScrollBar();

		virtual void InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg) = 0;
		virtual void SetPos(UOSInt pos) = 0;
		virtual UOSInt GetPos() = 0;
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn) = 0;
		virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void UpdatePos(Bool redraw) = 0;
		
		virtual Text::CStringNN GetObjectClass() const;
		void EventPosChanged(UOSInt newPos);
		void HandlePosChanged(PosChgEvent hdlr, AnyType userObj);
	};
}
#endif
