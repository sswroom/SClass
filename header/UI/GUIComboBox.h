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
		OSInt minVisible;
		Data::ArrayList<void *> *items;
		Data::ArrayList<const UTF8Char *> *itemTexts;
		Bool allowEdit;
		Bool autoComplete;
		UOSInt lastTextLeng;

	public:
		GUIComboBox(GUICore *ui, UI::GUIClientControl *parent, Bool allowEditing);
		virtual ~GUIComboBox();

		void EventSelectionChange();
		void EventTextChanged();

		virtual void SetText(const UTF8Char *text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(Text::StringBuilderUTF *sb);

		OSInt AddItem(const UTF8Char *itemText, void *itemObj);
		OSInt InsertItem(OSInt index, const UTF8Char *itemText, void *itemObj);
		void *RemoveItem(OSInt index);
		void ClearItems();
		OSInt GetCount();
		void SetSelectedIndex(OSInt index);
		OSInt GetSelectedIndex();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		void *GetSelectedItem();
		UTF8Char *GetItemText(UTF8Char *buff, OSInt index);
		void *GetItem(OSInt index);

		virtual void GetSize(Double *width, Double *height);
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);

		virtual void HandleSelectionChange(UI::UIEvent hdlr, void *userObj);

		virtual void UpdatePos(Bool redraw);

		void SetAutoComplete(Bool autoComplete);
		void SetTextSelection(OSInt startPos, OSInt endPos);
		OSInt GetListMinVisible();
		Bool SetListMinVisible(OSInt itemCount);
	private:
		OSInt GetSelectionHeight();
		Bool SetListItemHeight(OSInt itemHeight);
	};
};
#endif
