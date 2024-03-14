#ifndef _SM_UI_GUILISTBOX
#define _SM_UI_GUILISTBOX
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIListBox : public GUIControl
	{
	private:
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> selChgHdlrs;
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> dblClickHdlrs;
		Data::ArrayList<Data::CallbackStorage<UI::GUIControl::MouseEventHandler>> rightClickHdlrs;

	public:
		GUIListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIListBox();

		virtual Text::CStringNN GetObjectClass() const;

		void EventSelectionChange();
		void EventDoubleClick();
		void EventRightClick(Math::Coord2D<OSInt> pos);
		virtual void HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleDoubleClicked(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, AnyType userObj);

		virtual UOSInt AddItem(NotNullPtr<Text::String> itemText, AnyType itemObj) = 0;
		virtual UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual UOSInt InsertItem(UOSInt index, Text::String *itemText, AnyType itemObj) = 0;
		virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual AnyType RemoveItem(UOSInt index) = 0;
		virtual AnyType GetItem(UOSInt index) = 0;
		virtual void ClearItems() = 0;
		virtual UOSInt GetCount() = 0;
		virtual void SetSelectedIndex(UOSInt index) = 0;
		virtual UOSInt GetSelectedIndex() = 0;
		virtual Bool GetSelectedIndices(Data::ArrayList<UInt32> *indices) = 0;
		virtual AnyType GetSelectedItem() = 0;
		virtual UTF8Char *GetSelectedItemText(UTF8Char *buff) = 0;
		virtual Optional<Text::String> GetSelectedItemTextNew() = 0;
		virtual UTF8Char *GetItemText(UTF8Char *buff, UOSInt index) = 0;
		virtual void SetItemText(UOSInt index, Text::CStringNN text) = 0;
		virtual Optional<Text::String> GetItemTextNew(UOSInt index) = 0;
		virtual OSInt GetItemHeight() = 0;

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
	};
}
#endif
