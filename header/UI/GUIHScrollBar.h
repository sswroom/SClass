#ifndef _SM_UI_GUIHSCROLLBAR
#define _SM_UI_GUIHSCROLLBAR
#include "UI/GUIControl.h"

namespace UI
{
	class GUIHScrollBar : public GUIControl
	{
	public:
		typedef void (__stdcall *PosChgEvent)(void *userObj, OSInt newPos);

	private:
		Data::ArrayList<PosChgEvent> *posChgHdlrs;
		Data::ArrayList<void *> *posChgObjs;

	private:
		static Int32 useCnt;

		static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(void *hInst);
		static void Deinit(void *hInst);
	public:
		GUIHScrollBar(GUICore *ui, UI::GUIClientControl *parent, Int32 width);
		virtual ~GUIHScrollBar();

		void InitScrollBar(OSInt minVal, OSInt maxVal, OSInt currVal, OSInt largeChg);
		void SetPos(OSInt pos);
		OSInt GetPos();

		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
		virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn);
		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void UpdatePos(Bool redraw);
		virtual void EventPosChanged();

		virtual void HandlePosChanged(PosChgEvent hdlr, void *userObj);

		static Int32 GetSystemSize();
	};
};
#endif
