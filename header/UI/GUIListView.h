#ifndef _SM_UI_GUILISTVIEW
#define _SM_UI_GUILISTVIEW
#include "Media/DrawEngine.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIListView : public GUIControl
	{
	public:
		typedef enum
		{
			LVSTYLE_ICON,
			LVSTYLE_LIST,
			LVSTYLE_TABLE,
			LVSTYLE_SMALLICON
		} ListViewStyle;

		typedef void (__stdcall *ItemEvent)(void *userObj, OSInt itemIndex);
	protected:
		void *clsData;
	private:
		Data::ArrayList<UI::UIEvent> *selChgHdlrs;
		Data::ArrayList<void*> *selChgObjs;
		Data::ArrayList<ItemEvent> *dblClkHdlrs;
		Data::ArrayList<void*> *dblClkObjs;
		void *himgList;
		UInt32 imgW;
		UInt32 imgH;
		ListViewStyle lvStyle;
		Bool changing;
		UOSInt colCnt;

		static OSInt useCnt;

	public:
		GUIListView(GUICore *ui, UI::GUIClientControl *parent, ListViewStyle lvstyle, UOSInt colCount);
		virtual ~GUIListView();

		void ResetImages(UInt32 width, UInt32 height);
		UOSInt AddImage(Media::DrawImage *img);
		void EndAddingImage();

		void ChangeColumnCnt(UOSInt newColCnt);
		UOSInt GetColumnCnt();
		Bool AddColumn(const UTF8Char *columnName, Double colWidth);
		Bool AddColumn(const WChar *columnName, Double colWidth);
		Bool SetColumnWidth(UOSInt index, Double colWidth);
		Bool ClearAll();

		UOSInt AddItem(const UTF8Char *itemText, void *itemObj);
		UOSInt AddItem(const WChar *itemText, void *itemObj);
		UOSInt AddItem(const UTF8Char *itemText, void *itemObj, UOSInt imageIndex);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, const UTF8Char *text);
		Bool SetSubItem(UOSInt index, UOSInt subIndex, const WChar *text);
		Bool GetSubItem(UOSInt index, UOSInt subIndex, Text::StringBuilderUTF *sb);
		UOSInt InsertItem(UOSInt index, const UTF8Char *itemText, void *itemObj);
		UOSInt InsertItem(UOSInt index, const WChar *itemText, void *itemObj);
		void *RemoveItem(UOSInt index);
		void *GetItem(UOSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(OSInt index);
		OSInt GetSelectedIndex();
		UOSInt GetSelectedIndices(Data::ArrayList<UOSInt> *selIndices);
		void *GetSelectedItem();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		const UTF8Char *GetSelectedItemTextNew();
		UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
		const UTF8Char *GetItemTextNew(UOSInt index);
		void DelTextNew(const UTF8Char *text);
		void SetFullRowSelect(Bool fullRowSelect);
		void SetShowGrid(Bool showGrid);
		UOSInt GetStringWidth(const UTF8Char *s);
		UOSInt GetStringWidth(const WChar *s);
		void GetItemRectP(UOSInt index, Int32 *rect);
		void EnsureVisible(UOSInt index);

		void BeginUpdate();
		void EndUpdate();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
		void EventSelChg();
		void EventDblClk(OSInt index);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void HandleSelChg(UI::UIEvent hdlr, void *userObj);
		void HandleDblClk(ItemEvent hdlr, void *userObj);
	};
}
#endif
