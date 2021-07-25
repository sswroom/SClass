#ifndef _SM_UI_GUILISTBOX
#define _SM_UI_GUILISTBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIListBox : public GUIControl
	{
	private:
		Data::ArrayList<UI::UIEvent> *selChgHdlrs;
		Data::ArrayList<void *> *selChgObjs;
		Data::ArrayList<UI::UIEvent> *dblClickHdlrs;
		Data::ArrayList<void *> *dblClickObjs;
		Data::ArrayList<UI::GUIControl::MouseEventHandler> *rightClickHdlrs;
		Data::ArrayList<void *> *rightClickObjs;
		Data::ArrayList<void *> *items;
		void *clsData;
		Bool mulSel;

		static OSInt __stdcall LBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
	public:
		GUIListBox(GUICore *ui, UI::GUIClientControl *parent, Bool multiSelect);
		virtual ~GUIListBox();

		void EventSelectionChange();
		void EventDoubleClick();
		void EventRightClick(OSInt x, OSInt y);

		UOSInt AddItem(const UTF8Char *itemText, void *itemObj);
		UOSInt AddItem(const WChar *itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, const UTF8Char *itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, const WChar *itemText, void *itemObj);
		void *RemoveItem(UOSInt index);
		void *GetItem(UOSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(UOSInt index);
		UOSInt GetSelectedIndex();
		Bool GetSelectedIndices(Data::ArrayList<UInt32> *indices);
		void *GetSelectedItem();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		WChar *GetSelectedItemText(WChar *buff);
		const UTF8Char *GetSelectedItemTextNew();
		UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
		WChar *GetItemText(WChar *buff, UOSInt index);
		void SetItemText(UOSInt index, const UTF8Char *text);
		const UTF8Char *GetItemTextNew(UOSInt index);
		void DelTextNew(const UTF8Char *text);
		OSInt GetItemHeight();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleSelectionChange(UI::UIEvent hdlr, void *userObj);
		virtual void HandleDoubleClicked(UI::UIEvent hdlr, void *userObj);
		virtual void HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, void *userObj);
	};
}
#endif
