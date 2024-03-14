#ifndef _SM_UI_GUILISTVIEW
#define _SM_UI_GUILISTVIEW
#include "AnyType.h"
#include "Media/DrawEngine.h"
#include "Text/String.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIListView : public GUIControl
	{
	public:
		typedef void (__stdcall *ItemEvent)(AnyType userObj, UOSInt itemIndex);
		typedef void (__stdcall *MouseEvent)(AnyType userObj, Math::Coord2DDbl pos, UOSInt index);
	protected:
		void *clsData;
	private:
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> selChgHdlrs;
		Data::ArrayList<Data::CallbackStorage<ItemEvent>> dblClkHdlrs;
		Data::ArrayList<Data::CallbackStorage<MouseEvent>> rClkHdlrs;
		void *himgList;
		UInt32 imgW;
		UInt32 imgH;
		ListViewStyle lvStyle;
		Bool changing;
		UOSInt colCnt;

		static OSInt useCnt;

	public:
		GUIListView(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, ListViewStyle lvstyle, UOSInt colCount);
		virtual ~GUIListView();

		void ResetImages(UInt32 width, UInt32 height);
		UOSInt AddImage(Media::DrawImage *img);
		void EndAddingImage();

		void ChangeColumnCnt(UOSInt newColCnt);
		UOSInt GetColumnCnt();
		Bool AddColumn(NotNullPtr<Text::String> columnName, Double colWidth);
		Bool AddColumn(Text::CStringNN columnName, Double colWidth);
		Bool AddColumn(const WChar *columnName, Double colWidth);
		Bool SetColumnWidth(UOSInt index, Double colWidth);
		Bool ClearAll();

		UOSInt AddItem(NotNullPtr<Text::String> itemText, void *itemObj);
		UOSInt AddItem(Text::CStringNN itemText, void *itemObj);
		UOSInt AddItem(const WChar *itemText, void *itemObj);
		UOSInt AddItem(Text::CStringNN itemText, void *itemObj, UOSInt imageIndex);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, NotNullPtr<Text::String> text);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, Text::CStringNN text);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, const WChar *text);
		Bool GetSubItem(UOSInt index, UOSInt subIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
		UOSInt InsertItem(UOSInt index, NotNullPtr<Text::String> itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, const WChar *itemText, void *itemObj);
		void *RemoveItem(UOSInt index);
		void *GetItem(UOSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(UOSInt index);
		UOSInt GetSelectedIndex();
		UOSInt GetSelectedIndices(Data::ArrayList<UOSInt> *selIndices);
		void *GetSelectedItem();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		Text::String *GetSelectedItemTextNew();
		UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
		Text::String *GetItemTextNew(UOSInt index);
		void SetFullRowSelect(Bool fullRowSelect);
		void SetShowGrid(Bool showGrid);
		UOSInt GetStringWidth(const UTF8Char *s);
		UOSInt GetStringWidth(const WChar *s);
		void GetItemRectP(UOSInt index, Int32 *rect);
		void EnsureVisible(UOSInt index);
		void *SetItem(UOSInt index, void *itemObj);

		void BeginUpdate();
		void EndUpdate();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
		void EventSelChg();
		void EventDblClk(UOSInt index);
		void EventMouseClick(Math::Coord2DDbl coord, MouseButton btn);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void HandleSelChg(UI::UIEvent hdlr, AnyType userObj);
		void HandleDblClk(ItemEvent hdlr, AnyType userObj);
		void HandleRightClick(MouseEvent hdlr, AnyType userObj);
	};
}
#endif
