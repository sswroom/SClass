#ifndef _SM_UI_GUICOMBOBOX
#define _SM_UI_GUICOMBOBOX
#include "Data/ArrayListStringNN.h"
#include "Text/String.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIComboBox : public GUIControl
	{
	protected:
		Data::ArrayList<UI::UIEvent> selChgHdlrs;
		Data::ArrayList<AnyType> selChgObjs;
		Data::ArrayListStringNN itemTexts;
		Bool autoComplete;
		UOSInt lastTextLeng;
		Bool nonUIEvent;

	public:
		GUIComboBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIComboBox();

		void EventSelectionChange();
		void EventTextChanged();

		virtual void SetText(Text::CStringNN text) = 0;
		virtual UTF8Char *GetText(UTF8Char *buff) = 0;
		virtual Bool GetText(NotNullPtr<Text::StringBuilderUTF8> sb) = 0;

		virtual void BeginUpdate() = 0;
		virtual void EndUpdate() = 0;
		virtual UOSInt AddItem(NotNullPtr<Text::String> itemText, AnyType itemObj) = 0;
		virtual UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual UOSInt InsertItem(UOSInt index, NotNullPtr<Text::String> itemText, AnyType itemObj) = 0;
		virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual AnyType RemoveItem(UOSInt index) = 0;
		virtual void ClearItems() = 0;
		virtual UOSInt GetCount() = 0;
		virtual void SetSelectedIndex(UOSInt index) = 0;
		virtual UOSInt GetSelectedIndex() = 0;
		virtual AnyType GetSelectedItem() = 0;
		virtual AnyType GetItem(UOSInt index) = 0;

		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
		virtual Text::CStringNN GetObjectClass() const;

		virtual Math::Size2DDbl GetSize() = 0;
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void UpdatePos(Bool redraw) = 0;
		virtual void SetTextSelection(UOSInt startPos, UOSInt endPos) = 0;

		virtual void HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj);
		void SetAutoComplete(Bool autoComplete);
	};
}
#endif
