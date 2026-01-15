#ifndef _SM_UI_GUITEXTVIEW
#define _SM_UI_GUITEXTVIEW
#include "Media/DrawEngine.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextView : public GUIControl
	{
	private:
		struct ClassData;

		static IntOS useCnt;
		NN<Media::DrawEngine> deng;
		ClassData *clsData;
		void *drawFont;
		Optional<Media::DrawImage> drawBuff;
		Optional<Media::ColorSess> colorSess;

	protected:	
		UInt32 pageLineCnt;
		Double pageLineHeight;
		UInt32 bgColor;
		UInt32 scrColor;
		UInt32 txtColor;
		UInt32 lineNumColor;
		UInt32 selColor;
		UInt32 selTextColor;

	private:
		static IntOS __stdcall TFVWndProc(void *hWnd, UInt32 msg, UInt32 wParam, IntOS lParam);
		static void __stdcall OnResize(AnyType userObj);
		void Init(Optional<InstanceHandle> hInst);
		void Deinit(Optional<InstanceHandle> hInst);
		void OnPaint();
		void UpdateScrollBar();

	protected:
		Bool IsShiftPressed();
		void SetScrollHPos(UIntOS pos, Bool redraw);
		void SetScrollVPos(UIntOS pos, Bool redraw);
		void SetScrollHRange(UIntOS min, UIntOS max);
		void SetScrollVRange(UIntOS min, UIntOS max);

		UInt32 GetCharCntAtWidth(UnsafeArray<const WChar> str, UIntOS strLen, UIntOS pxWidth);
		void GetDrawSize(UnsafeArray<const WChar> str, UIntOS strLen, OutParam<UIntOS> width, OutParam<UIntOS> height);
		void SetCaretPos(IntOS scnX, IntOS scnY);
	public:
		GUITextView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess);
		virtual ~GUITextView();

		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);
		virtual void UpdateFont();
		virtual IntOS GetScrollHPos();
		virtual IntOS GetScrollVPos();

		virtual void EventLineUp() = 0;
		virtual void EventLineDown() = 0;
		virtual void EventPageUp() = 0;
		virtual void EventPageDown() = 0;
		virtual void EventLeft() = 0;
		virtual void EventRight() = 0;
		virtual void EventHome() = 0;
		virtual void EventEnd() = 0;
		virtual void EventLineBegin() = 0;
		virtual void EventLineEnd() = 0;
		virtual void EventCopy() = 0;
		virtual void EventMouseDown(IntOS scnX, IntOS scnY, MouseButton btn) = 0;
		virtual void EventMouseUp(IntOS scnX, IntOS scnY, MouseButton btn) = 0;
		virtual void EventMouseMove(IntOS scnX, IntOS scnY) = 0;
		virtual void EventTimerTick() = 0;
		virtual void DrawImage(NN<Media::DrawImage> dimg) = 0;
		virtual void UpdateCaretPos() = 0;

		void OnMouseDown(IntOS scnX, IntOS scnY, MouseButton btn);
		void OnMouseUp(IntOS scnX, IntOS scnY, MouseButton btn);
		void OnMouseMove(IntOS scnX, IntOS scnY);
		void OnMouseWheel(Bool isDown);
		void OnDraw(void *cr);
		void SetShiftState(Bool isDown);
		void FontUpdated()
		{
			this->UpdateScrollBar();
		}
	};
}
#endif
