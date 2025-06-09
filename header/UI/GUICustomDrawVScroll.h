#ifndef _SM_UI_GUICUSTOMDRAWVSCROLL
#define _SM_UI_GUICUSTOMDRAWVSCROLL
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Media/DrawEngine.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICustomDrawVScroll : public UI::GUIControl
	{
	public:
		struct ClassData;

		NN<Media::DrawEngine> deng;
		Optional<Media::ColorSess> colorSess;
		NN<ClassData> clsData;
		typedef enum
		{
			KBTN_NONE = 0,
			KBTN_SHIFT = 4,
			KBTN_CONTROL = 8
		} KeyButton;
	private:
		static Int32 useCnt;
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> selChgHdlrs;
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> dblClkHdlrs;

	private:
		static OSInt __stdcall CDVSWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		void OnPaint();
		void Init(Optional<InstanceHandle> hInst);
		void Deinit(Optional<InstanceHandle> hInst);
	
	protected:
		void ClearBackground(NN<Media::DrawImage> img);

	public:
		GUICustomDrawVScroll(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess);
		virtual ~GUICustomDrawVScroll();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

		virtual void OnDraw(NN<Media::DrawImage> img) = 0;
		virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
		virtual void OnKeyDown(UInt32 keyCode);

		void HandleSelChg(UI::UIEvent hdlr, AnyType userObj);
		void HandleDblClk(UI::UIEvent hdlr, AnyType userObj);
		void EventSelChg();
		void EventDblClk();
		void SetVScrollBar(UOSInt min, UOSInt max, UOSInt pageSize);
		UOSInt GetVScrollPos();
		Bool MakeVisible(UOSInt firstIndex, UOSInt lastIndex);
	};
}
#endif
