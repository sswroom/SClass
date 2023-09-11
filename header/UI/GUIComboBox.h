#ifndef _SM_UI_GUICOMBOBOX
#define _SM_UI_GUICOMBOBOX
#include "Data/ArrayListNN.h"
#include "Text/String.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIComboBox : public GUIControl
	{
	private:
		struct ClassData;
		ClassData *clsData;
		
		Data::ArrayList<UI::UIEvent> selChgHdlrs;
		Data::ArrayList<void *> selChgObjs;
		UOSInt minVisible;
		Data::ArrayList<void *> items;
		Data::ArrayListNN<Text::String> itemTexts;
		Bool allowEdit;
		Bool autoComplete;
		UOSInt lastTextLeng;
		Bool nonUIEvent;

	public:
		GUIComboBox(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Bool allowEditing);
		virtual ~GUIComboBox();

		void EventSelectionChange();
		void EventTextChanged();

		virtual void SetText(Text::CStringNN text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(NotNullPtr<Text::StringBuilderUTF8> sb);

		void BeginUpdate();
		void EndUpdate();
		UOSInt AddItem(NotNullPtr<Text::String> itemText, void *itemObj);
		UOSInt AddItem(Text::CStringNN itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, Text::String *itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj);
		void *RemoveItem(UOSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(UOSInt index);
		UOSInt GetSelectedIndex();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		void *GetSelectedItem();
		UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
		void *GetItem(UOSInt index);

		virtual Math::Size2DDbl GetSize();
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);

		virtual Text::CStringNN GetObjectClass() const;
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
