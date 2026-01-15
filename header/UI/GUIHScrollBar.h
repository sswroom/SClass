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
		typedef void (CALLBACKFUNC PosChgEvent)(AnyType userObj, UIntOS newPos);

	private:
		Data::ArrayListObj<Data::CallbackStorage<PosChgEvent>> posChgHdlrs;
	
	public:
		GUIHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIHScrollBar();

		virtual void InitScrollBar(UIntOS minVal, UIntOS maxVal, UIntOS currVal, UIntOS largeChg) = 0;
		virtual void SetPos(UIntOS pos) = 0;
		virtual UIntOS GetPos() = 0;
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn) = 0;
		virtual void SetAreaP(IntOS left, IntOS top, IntOS right, IntOS bottom, Bool updateScn) = 0;
		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
		virtual void UpdatePos(Bool redraw) = 0;
		
		virtual Text::CStringNN GetObjectClass() const;
		void EventPosChanged(UIntOS newPos);
		void HandlePosChanged(PosChgEvent hdlr, AnyType userObj);
	};
}
#endif
