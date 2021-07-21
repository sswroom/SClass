#ifndef _SM_UI_GUICHECKEDLISTBOX
#define _SM_UI_GUICHECKEDLISTBOX
#include "UI/GUIListView.h"

namespace UI
{
	class GUICheckedListBox : public GUIListView
	{
	private:
/*		Data::ArrayList<UI::UIEvent> *selChgHdlrs;
		Data::ArrayList<void *> *selChgObjs;
		Data::ArrayList<UI::UIEvent> *dblClickHdlrs;
		Data::ArrayList<void *> *dblClickObjs;
		Data::ArrayList<UI::UIEvent> *rightClickHdlrs;
		Data::ArrayList<void *> *rightClickObjs;
		void *oriWndProc;*/

//		static OSInt __stdcall LBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
	public:
		GUICheckedListBox(GUICore *ui, UI::GUIClientControl *parent);
		virtual ~GUICheckedListBox();

//		void EventSelectionChange();
//		void EventDoubleClick();
//		void EventRightClick();

		Bool GetItemChecked(UOSInt index);
		void SetItemChecked(UOSInt index, Bool isChecked);

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

//		virtual void HandleSelectionChange(UI::UIEvent hdlr, void *userObj);
//		virtual void HandleDoubleClicked(UI::UIEvent hdlr, void *userObj);
//		virtual void HandleRightClicked(UI::UIEvent hdlr, void *userObj);
	};
};
#endif
