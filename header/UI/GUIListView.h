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
		typedef void (CALLBACKFUNC ItemEvent)(AnyType userObj, UIntOS itemIndex);
		typedef void (CALLBACKFUNC MouseEvent)(AnyType userObj, Math::Coord2DDbl pos, UIntOS index);
	protected:
		void *clsData;
	private:
		Data::ArrayListObj<Data::CallbackStorage<UI::UIEvent>> selChgHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<ItemEvent>> dblClkHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<MouseEvent>> rClkHdlrs;
		void *himgList;
		UInt32 imgW;
		UInt32 imgH;
		ListViewStyle lvStyle;
		Bool changing;
		UIntOS colCnt;

		static IntOS useCnt;

	public:
		GUIListView(NN<GUICore> ui, NN<UI::GUIClientControl> parent, ListViewStyle lvstyle, UIntOS colCount);
		virtual ~GUIListView();

		void ResetImages(UInt32 width, UInt32 height);
		UIntOS AddImage(Media::DrawImage *img);
		void EndAddingImage();

		void ChangeColumnCnt(UIntOS newColCnt);
		UIntOS GetColumnCnt();
		Bool AddColumn(NN<Text::String> columnName, Double colWidth);
		Bool AddColumn(Text::CStringNN columnName, Double colWidth);
		Bool AddColumn(UnsafeArray<const WChar> columnName, Double colWidth);
		Bool SetColumnWidth(UIntOS index, Double colWidth);
		Bool ClearAll();

		UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj);
		UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj);
		UIntOS AddItem(UnsafeArray<const WChar> itemText, AnyType itemObj);
		UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj, UIntOS imageIndex);
		Bool SetSubItem(UIntOS index, UIntOS subIndex, NN<Text::String> text);
		Bool SetSubItem(UIntOS index, UIntOS subIndex, Text::CStringNN text);
		Bool SetSubItem(UIntOS index, UIntOS subIndex, UnsafeArray<const WChar> text);
		Bool GetSubItem(UIntOS index, UIntOS subIndex, NN<Text::StringBuilderUTF8> sb);
		UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj);
		UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj);
		UIntOS InsertItem(UIntOS index, UnsafeArray<const WChar> itemText, AnyType itemObj);
		AnyType RemoveItem(UIntOS index);
		AnyType GetItem(UIntOS index);
		void ClearItems();
		UIntOS GetCount();
		void SetSelectedIndex(UIntOS index);
		UIntOS GetSelectedIndex();
		UIntOS GetSelectedIndices(Data::ArrayListNative<UIntOS> *selIndices);
		AnyType GetSelectedItem();
		UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff);
		Optional<Text::String> GetSelectedItemTextNew();
		UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UIntOS index);
		Optional<Text::String> GetItemTextNew(UIntOS index);
		void SetFullRowSelect(Bool fullRowSelect);
		void SetShowGrid(Bool showGrid);
		UIntOS GetStringWidth(UnsafeArray<const UTF8Char> s);
		UIntOS GetStringWidth(const WChar *s);
		void GetItemRectP(UIntOS index, Int32 *rect);
		void EnsureVisible(UIntOS index);
		AnyType SetItem(UIntOS index, AnyType itemObj);

		void BeginUpdate();
		void EndUpdate();

		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
		void EventSelChg();
		void EventDblClk(UIntOS index);
		void EventMouseClick(Math::Coord2DDbl coord, MouseButton btn);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void HandleSelChg(UI::UIEvent hdlr, AnyType userObj);
		void HandleDblClk(ItemEvent hdlr, AnyType userObj);
		void HandleRightClick(MouseEvent hdlr, AnyType userObj);
	};
}
#endif
