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
		typedef void (CALLBACKFUNC ItemEvent)(AnyType userObj, UOSInt itemIndex);
		typedef void (CALLBACKFUNC MouseEvent)(AnyType userObj, Math::Coord2DDbl pos, UOSInt index);
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
		GUIListView(NN<GUICore> ui, NN<UI::GUIClientControl> parent, ListViewStyle lvstyle, UOSInt colCount);
		virtual ~GUIListView();

		void ResetImages(UInt32 width, UInt32 height);
		UOSInt AddImage(Media::DrawImage *img);
		void EndAddingImage();

		void ChangeColumnCnt(UOSInt newColCnt);
		UOSInt GetColumnCnt();
		Bool AddColumn(NN<Text::String> columnName, Double colWidth);
		Bool AddColumn(Text::CStringNN columnName, Double colWidth);
		Bool AddColumn(UnsafeArray<const WChar> columnName, Double colWidth);
		Bool SetColumnWidth(UOSInt index, Double colWidth);
		Bool ClearAll();

		UOSInt AddItem(NN<Text::String> itemText, AnyType itemObj);
		UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj);
		UOSInt AddItem(UnsafeArray<const WChar> itemText, AnyType itemObj);
		UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj, UOSInt imageIndex);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, NN<Text::String> text);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, Text::CStringNN text);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, UnsafeArray<const WChar> text);
		Bool GetSubItem(UOSInt index, UOSInt subIndex, NN<Text::StringBuilderUTF8> sb);
		UOSInt InsertItem(UOSInt index, NN<Text::String> itemText, AnyType itemObj);
		UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj);
		UOSInt InsertItem(UOSInt index, UnsafeArray<const WChar> itemText, AnyType itemObj);
		AnyType RemoveItem(UOSInt index);
		AnyType GetItem(UOSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(UOSInt index);
		UOSInt GetSelectedIndex();
		UOSInt GetSelectedIndices(Data::ArrayList<UOSInt> *selIndices);
		AnyType GetSelectedItem();
		UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff);
		Text::String *GetSelectedItemTextNew();
		UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UOSInt index);
		Text::String *GetItemTextNew(UOSInt index);
		void SetFullRowSelect(Bool fullRowSelect);
		void SetShowGrid(Bool showGrid);
		UOSInt GetStringWidth(UnsafeArray<const UTF8Char> s);
		UOSInt GetStringWidth(const WChar *s);
		void GetItemRectP(UOSInt index, Int32 *rect);
		void EnsureVisible(UOSInt index);
		AnyType SetItem(UOSInt index, AnyType itemObj);

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
