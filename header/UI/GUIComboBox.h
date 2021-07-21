#ifndef _SM_UI_GUICOMBOBOX
#define _SM_UI_GUICOMBOBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIComboBox : public GUIControl
	{
	private:
		Data::ArrayList<UI::UIEvent> *selChgHdlrs;
		Data::ArrayList<void *> *selChgObjs;
		UOSInt minVisible;
		Data::ArrayList<void *> *items;
		Data::ArrayList<const UTF8Char *> *itemTexts;
		Bool allowEdit;
		Bool autoComplete;
		UOSInt lastTextLeng;
		Bool nonUIEvent;

	public:
		GUIComboBox(GUICore *ui, UI::GUIClientControl *parent, Bool allowEditing);
		virtual ~GUIComboBox();

		void EventSelectionChange();
		void EventTextChanged();

		virtual void SetText(const UTF8Char *text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(Text::StringBuilderUTF *sb);

		UOSInt AddItem(const UTF8Char *itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, const UTF8Char *itemText, void *itemObj);
		void *RemoveItem(UOSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(UOSInt index);
		OSInt GetSelectedIndex();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		void *GetSelectedItem();
		UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
		void *GetItem(UOSInt index);

		virtual void GetSize(Double *width, Double *height);
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleSelectionChange(UI::UIEvent hdlr, void *userObj);

		virtual void UpdatePos(Bool redraw);

		void SetAutoComplete(Bool autoComplete);
		void SetTextSelection(UOSInt startPos, UOSInt endPos);
		UOSInt GetListMinVisible();
		Bool SetListMinVisible(UOSInt itemCount);
	private:
		UOSInt GetSelectionHeight();
		Bool SetListItemHeight(UOSInt itemHeight);
	};
}
#endif
