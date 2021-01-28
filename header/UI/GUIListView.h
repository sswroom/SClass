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
		Int32 imgW;
		Int32 imgH;
		ListViewStyle lvStyle;
		Bool changing;
		OSInt colCnt;

		static OSInt useCnt;

	public:
		GUIListView(GUICore *ui, UI::GUIClientControl *parent, ListViewStyle lvstyle, OSInt colCount);
		virtual ~GUIListView();

		void ResetImages(Int32 width, Int32 height);
		OSInt AddImage(Media::DrawImage *img);
		void EndAddingImage();

		void ChangeColumnCnt(OSInt newColCnt);
		OSInt GetColumnCnt();
		Bool AddColumn(const UTF8Char *columnName, Double colWidth);
		Bool AddColumn(const WChar *columnName, Double colWidth);
		Bool SetColumnWidth(OSInt index, Double colWidth);
		Bool ClearAll();

		OSInt AddItem(const UTF8Char *itemText, void *itemObj);
		OSInt AddItem(const WChar *itemText, void *itemObj);
		OSInt AddItem(const UTF8Char *itemText, void *itemObj, OSInt imageIndex);
		Bool SetSubItem(OSInt index, OSInt subIndex, const UTF8Char *text);
		Bool SetSubItem(OSInt index, OSInt subIndex, const WChar *text);
		Bool GetSubItem(OSInt index, OSInt subIndex, Text::StringBuilderUTF *sb);
		OSInt InsertItem(OSInt index, const UTF8Char *itemText, void *itemObj);
		OSInt InsertItem(OSInt index, const WChar *itemText, void *itemObj);
		void *RemoveItem(OSInt index);
		void *GetItem(OSInt index);
		void ClearItems();
		UOSInt GetCount();
		void SetSelectedIndex(OSInt index);
		OSInt GetSelectedIndex();
		OSInt GetSelectedIndices(Data::ArrayList<OSInt> *selIndices);
		void *GetSelectedItem();
		UTF8Char *GetSelectedItemText(UTF8Char *buff);
		const UTF8Char *GetSelectedItemTextNew();
		UTF8Char *GetItemText(UTF8Char *buff, OSInt index);
		const UTF8Char *GetItemTextNew(OSInt index);
		void DelTextNew(const UTF8Char *text);
		void SetFullRowSelect(Bool fullRowSelect);
		void SetShowGrid(Bool showGrid);
		OSInt GetStringWidth(const UTF8Char *s);
		OSInt GetStringWidth(const WChar *s);
		void GetItemRectP(OSInt index, Int32 *rect);
		void EnsureVisible(OSInt index);

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
};
#endif
