#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIListView.h"
#include <windows.h>
#include <commctrl.h>

#ifndef ILC_COLOR24
#define ILC_COLOR24 ILC_COLOR
#endif

OSInt UI::GUIListView::useCnt = 0;

UI::GUIListView::GUIListView(UI::GUICore *ui, UI::GUIClientControl *parent, ListViewStyle lvstyle, UOSInt colCount) : UI::GUIControl(ui, parent)
{
	Double w;
	Double h;
	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);
	}
	this->colCnt = 0;

	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->dblClkHdlrs, Data::ArrayList<ItemEvent>());
	NEW_CLASS(this->dblClkObjs, Data::ArrayList<void*>());
	this->himgList = 0;
	this->lvStyle = lvstyle;
	parent->GetClientSize(&w, &h);
	UInt32 style = WS_TABSTOP | WS_CHILD | WS_VSCROLL | WS_BORDER;
	if (lvstyle == LVSTYLE_ICON)
	{
		style |= LVS_ICON;
	}
	else if (lvstyle == LVSTYLE_LIST)
	{
		style |= LVS_LIST;
	}
	else if (lvstyle == LVSTYLE_SMALLICON)
	{
		style |= LVS_SMALLICON;
	}
	else if (lvstyle == LVSTYLE_TABLE)
	{
		style |= LVS_REPORT;
	}


	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, WC_LISTVIEWW, (const UTF8Char*)"ListView", style, WS_EX_CLIENTEDGE, 0, 0, w, h);
}

UI::GUIListView::~GUIListView()
{
	if (this->himgList)
	{
		ImageList_Destroy((HIMAGELIST)this->himgList);
	}
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
	DEL_CLASS(this->dblClkHdlrs);
	DEL_CLASS(this->dblClkObjs);
}

void UI::GUIListView::ResetImages(UInt32 width, UInt32 height)
{
	if (this->himgList)
	{
		ImageList_Destroy((HIMAGELIST)this->himgList);
	}
	this->himgList = ImageList_Create((int)width, (int)height, ILC_COLOR24, 0, 0);
	this->imgW = width;
	this->imgH = height;
}

UOSInt UI::GUIListView::AddImage(Media::DrawImage *img)
{
//	IMAGEINFO imgInfo;
	if (this->himgList == 0)
		return INVALID_INDEX;
	OSInt retIndex = ImageList_AddMasked((HIMAGELIST)this->himgList, (HBITMAP)((Media::GDIImage*)img)->hBmp, 0);
/*	ImageList_GetImageInfo((HIMAGELIST)this->himgList, retIndex, &imgInfo);

	UInt8 buff[2048];
	BITMAPINFO bmi;
	Int32 ret;
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biBitCount = 0;
	ret = GetDIBits(GetDC(0), imgInfo.hbmImage, 0, 0, 0, &bmi, DIB_RGB_COLORS);
	ret = GetDIBits(GetDC(0), imgInfo.hbmImage, 0, 1, buff, &bmi, DIB_RGB_COLORS);*/

	return (UOSInt)retIndex;
}

void UI::GUIListView::EndAddingImage()
{
	if (this->lvStyle == UI::GUIListView::LVSTYLE_SMALLICON)
	{
		SendMessage((HWND)this->hwnd, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)this->himgList);
	}
	else
	{
		SendMessage((HWND)this->hwnd, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)this->himgList);
	}
}

void UI::GUIListView::ChangeColumnCnt(UOSInt newColCnt)
{
}

UOSInt UI::GUIListView::GetColumnCnt()
{
	return this->colCnt;
}

Bool UI::GUIListView::AddColumn(Text::String *columnName, Double colWidth)
{
	const WChar *wptr = Text::StrToWCharNew(columnName->v);
	Bool ret = this->AddColumn(wptr, colWidth);
	Text::StrDelNew(wptr);
	return ret;
}

Bool UI::GUIListView::AddColumn(const UTF8Char *columnName, Double colWidth)
{
	const WChar *wptr = Text::StrToWCharNew(columnName);
	Bool ret = this->AddColumn(wptr, colWidth);
	Text::StrDelNew(wptr);
	return ret;
}

Bool UI::GUIListView::AddColumn(const WChar *columnName, Double colWidth)
{
	LVCOLUMNW col;
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.pszText = (LPWSTR)columnName;
	col.cx = Double2Int32(colWidth * this->hdpi / this->ddpi);

	OSInt ret = SendMessage((HWND)this->hwnd, LVM_INSERTCOLUMNW, this->colCnt, (LPARAM)&col);
	if (ret != -1)
	{
		this->colCnt++;
		return true;
	}
	return false;
}

Bool UI::GUIListView::SetColumnWidth(UOSInt index, Double colWidth)
{
	LVCOLUMNW col;
	col.mask = LVCF_WIDTH;
	col.cx = Double2Int32(colWidth * this->hdpi / this->ddpi);

	OSInt ret = SendMessage((HWND)this->hwnd, LVM_SETCOLUMNW, index, (LPARAM)&col);
	if (ret != -1)
	{
		return true;
	}
	return false;
}

Bool UI::GUIListView::ClearAll()
{
	while (this->colCnt > 0)
	{
		this->colCnt--;
		SendMessage((HWND)this->hwnd, LVM_DELETECOLUMN, this->colCnt, 0);
	}
	this->ClearItems();
	return true;
}

UOSInt UI::GUIListView::AddItem(Text::String *itemText, void *itemObj)
{
	UOSInt strLen = Text::StrUTF8_WCharCntC(itemText->v, itemText->leng);
	WChar *ws = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WCharC(ws, itemText->v, itemText->leng, 0);
	LVITEMW item;
	item.iItem = (Int32)GetCount();
	item.iSubItem = 0;
	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.lParam = (LPARAM)itemObj;
	item.pszText = (LPWSTR)ws;
	item.cchTextMax = 256;
	strLen = (UOSInt)SendMessage((HWND)this->hwnd, LVM_INSERTITEMW, 0, (LPARAM)&item);
	Text::StrDelNew(ws);
	return strLen;
}

UOSInt UI::GUIListView::AddItem(const UTF8Char *itemText, void *itemObj)
{
	UOSInt strLen = Text::StrUTF8_WCharCnt(itemText);
	WChar *ws = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(ws, itemText, 0);
	LVITEMW item;
	item.iItem = (Int32)GetCount();
	item.iSubItem = 0;
	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.lParam = (LPARAM)itemObj;
	item.pszText = (LPWSTR)ws;
	item.cchTextMax = 256;
	strLen = (UOSInt)SendMessage((HWND)this->hwnd, LVM_INSERTITEMW, 0, (LPARAM)&item);
	Text::StrDelNew(ws);
	return strLen;
}

UOSInt UI::GUIListView::AddItem(const WChar *itemText, void *itemObj)
{
	LVITEMW item;
	item.iItem = (Int32)GetCount();
	item.iSubItem = 0;
	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.lParam = (LPARAM)itemObj;
	item.pszText = (LPWSTR)itemText;
	item.cchTextMax = 256;
	return (UOSInt)SendMessage((HWND)this->hwnd, LVM_INSERTITEMW, 0, (LPARAM)&item);
}

UOSInt UI::GUIListView::AddItem(const UTF8Char *itemText, void *itemObj, UOSInt imageIndex)
{
	LVITEMW item;
	item.iItem = (Int32)GetCount();
	item.iSubItem = 0;
	item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
	item.lParam = (LPARAM)itemObj;
	item.pszText = (LPWSTR)Text::StrToWCharNew(itemText);
	item.cchTextMax = (int)Text::StrCharCnt(itemText);
	item.iImage = (Int32)imageIndex;
	UOSInt ret = (UOSInt)SendMessage((HWND)this->hwnd, LVM_INSERTITEM, 0, (LPARAM)&item);
	Text::StrDelNew((const WChar*)item.pszText);
	return ret;
}

Bool UI::GUIListView::SetSubItem(UOSInt index, UOSInt subIndex, Text::String *text)
{
	const WChar *ws = 0;
	LVITEMW item;
	item.iItem = (int)index;
	item.iSubItem = (int)subIndex;
	item.mask = LVIF_TEXT;
	if (text != 0)
	{
		ws = Text::StrToWCharNew(text->v);
		item.pszText = (LPWSTR)ws;
		item.cchTextMax = (int)Text::StrCharCnt(ws);
	}
	else
	{
		item.pszText = 0;
		item.cchTextMax = 0;
	}
	Bool ret = (SendMessage((HWND)this->hwnd, LVM_SETITEMW, 0, (LPARAM)&item) == TRUE);
	SDEL_TEXT(ws);
	return ret;
}

Bool UI::GUIListView::SetSubItem(UOSInt index, UOSInt subIndex, const UTF8Char *text)
{
	const WChar *ws = 0;
	LVITEMW item;
	item.iItem = (int)index;
	item.iSubItem = (int)subIndex;
	item.mask = LVIF_TEXT;
	if (text != 0)
	{
		ws = Text::StrToWCharNew(text);
		item.pszText = (LPWSTR)ws;
		item.cchTextMax = (int)Text::StrCharCnt(ws);
	}
	else
	{
		item.pszText = 0;
		item.cchTextMax = 0;
	}
	Bool ret = (SendMessage((HWND)this->hwnd, LVM_SETITEMW, 0, (LPARAM)&item) == TRUE);
	SDEL_TEXT(ws);
	return ret;
}

Bool UI::GUIListView::SetSubItem(UOSInt index, UOSInt subIndex, const WChar *text)
{
	LVITEMW item;
	item.iItem = (int)index;
	item.iSubItem = (int)subIndex;
	item.mask = LVIF_TEXT;
	item.pszText = (LPWSTR)text;
	item.cchTextMax = 256;
	return (SendMessage((HWND)this->hwnd, LVM_SETITEMW, 0, (LPARAM)&item) == TRUE);
}

Bool UI::GUIListView::GetSubItem(UOSInt index, UOSInt subIndex, Text::StringBuilderUTF8 *sb)
{
	LVITEMW item;
	sb->AllocLeng(256);
	item.iItem = (int)index;
	item.iSubItem = (int)subIndex;
	item.mask = LVIF_TEXT;
	item.pszText = MemAlloc(WCHAR, 256);
	item.cchTextMax = 256;
	Bool ret = (SendMessage((HWND)this->hwnd, LVM_GETITEMW, 0, (LPARAM)&item) == TRUE);
	if (ret)
	{
		sb->AppendW(item.pszText);
	}
	else
	{
	}
	MemFree(item.pszText);
	return ret;
}

UOSInt UI::GUIListView::InsertItem(UOSInt index, const UTF8Char *itemText, void *itemObj)
{
	LVITEMW item;
	item.iItem = (Int32)index;
	item.iSubItem = 0;
	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.lParam = (LPARAM)itemObj;
	const WChar *wptr = Text::StrToWCharNew(itemText);
	item.pszText = (LPWSTR)wptr;
	item.cchTextMax = 256;
	UOSInt ret = (UOSInt)SendMessage((HWND)this->hwnd, LVM_INSERTITEM, 0, (LPARAM)&item);
	Text::StrDelNew(wptr);
	return ret;
}

UOSInt UI::GUIListView::InsertItem(UOSInt index, const WChar *itemText, void *itemObj)
{
	LVITEMW item;
	item.iItem = (Int32)index;
	item.iSubItem = 0;
	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.lParam = (LPARAM)itemObj;
	item.pszText = (LPWSTR)itemText;
	item.cchTextMax = 256;
	return (UOSInt)SendMessage((HWND)this->hwnd, LVM_INSERTITEM, 0, (LPARAM)&item);
}

void *UI::GUIListView::RemoveItem(UOSInt index)
{
	void *item = GetItem(index);
	SendMessage((HWND)this->hwnd, LVM_DELETEITEM, index, 0);
	return item;
}

void *UI::GUIListView::GetItem(UOSInt index)
{
	LVITEM item;
	item.iItem = (Int32)index;
	item.iSubItem = 0;
	item.mask = LVIF_PARAM;
	item.lParam = 0;
    SendMessage((HWND)this->hwnd, LVM_GETITEM, 0, (LPARAM)&item);
	return (void*)item.lParam;
}

void UI::GUIListView::ClearItems()
{
	SendMessage((HWND)this->hwnd, LVM_DELETEALLITEMS, 0, 0);
}

UOSInt UI::GUIListView::GetCount()
{
	return (UOSInt)SendMessage((HWND)this->hwnd, LVM_GETITEMCOUNT, 0, 0);
}

void UI::GUIListView::SetSelectedIndex(UOSInt index)
{
	SendMessage((HWND)this->hwnd, LVM_SETSELECTIONMARK, 0, (LPARAM)index);
	this->EventSelChg();
}

UOSInt UI::GUIListView::GetSelectedIndex()
{
	return (UOSInt)SendMessage((HWND)this->hwnd, LVM_GETSELECTIONMARK, 0, 0);
}

UOSInt UI::GUIListView::GetSelectedIndices(Data::ArrayList<UOSInt> *selIndices)
{
	UOSInt cnt = (UOSInt)SendMessage((HWND)this->hwnd, LVM_GETITEMCOUNT, 0, 0);
	UOSInt i;
	UOSInt ret = 0;
	i = 0;
	while (i < cnt)
	{
		if (SendMessage((HWND)this->hwnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) != 0)
		{
			selIndices->Add(i);
			ret++;
		}
		i++;
	}
	return ret;
}

void *UI::GUIListView::GetSelectedItem()
{
	UOSInt i = GetSelectedIndex();
	if (i != INVALID_INDEX)
		return this->GetItem(i);
	return 0;
}

UTF8Char *UI::GUIListView::GetSelectedItemText(UTF8Char *buff)
{
	UOSInt i = GetSelectedIndex();
	if (i != INVALID_INDEX)
		return this->GetItemText(buff, i);
	return 0;
}

Text::String *UI::GUIListView::GetSelectedItemTextNew()
{
	UOSInt i = GetSelectedIndex();
	if (i != INVALID_INDEX)
		return this->GetItemTextNew(i);
	return 0;
}

UTF8Char *UI::GUIListView::GetItemText(UTF8Char *buff, UOSInt index)
{
	WChar sbuff[256];
	sbuff[0] = 0;
	LVITEMW item;
	item.iItem = (Int32)index;
	item.iSubItem = 0;
	item.cchTextMax = 256;
	item.pszText = sbuff;
	SendMessage((HWND)this->hwnd, LVM_GETITEMTEXTW, index, (LPARAM)&item);
	return Text::StrWChar_UTF8(buff, sbuff);
}

Text::String *UI::GUIListView::GetItemTextNew(UOSInt index)
{
	UTF8Char sbuff[768];
	Text::String *sout;
	UTF8Char *sptr = GetItemText(sbuff, index);
	if (sptr == 0)
		return 0;
	sout = Text::String::New((UOSInt)(sptr - sbuff));
	MemCopyNO(sout->v, sbuff, sizeof(UTF8Char) * (UOSInt)(sptr - sbuff + 1));
	return sout;
}

void UI::GUIListView::SetFullRowSelect(Bool fullRowSelect)
{
	SendMessage((HWND)this->hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, fullRowSelect?LVS_EX_FULLROWSELECT:0);
}

void UI::GUIListView::SetShowGrid(Bool showGrid)
{
	SendMessage((HWND)this->hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES, showGrid?LVS_EX_GRIDLINES:0);
}

UOSInt UI::GUIListView::GetStringWidth(const UTF8Char *s)
{
	UOSInt strLen = Text::StrUTF8_WCharCnt(s);
	WChar *ws = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(ws, s, 0);
	strLen = (UOSInt)SendMessage((HWND)this->hwnd, LVM_GETSTRINGWIDTHW, 0, (LPARAM)ws);
	MemFree(ws);
	return strLen;
}

UOSInt UI::GUIListView::GetStringWidth(const WChar *s)
{
	return (UOSInt)SendMessage((HWND)this->hwnd, LVM_GETSTRINGWIDTHW, 0, (LPARAM)s);
}

void UI::GUIListView::GetItemRectP(UOSInt index, Int32 *rect)
{
	RECT rc;
	rc.left = LVIR_BOUNDS;
	if (SendMessage((HWND)this->hwnd, LVM_GETITEMRECT, index, (LPARAM)&rc) == TRUE)
	{
		rect[0] = rc.left;
		rect[1] = rc.top;
		rect[2] = rc.right - rc.left;
		rect[3] = rc.bottom - rc.top;
	}
	else
	{
		rect[0] = 0;
		rect[1] = 0;
		rect[2] = 0;
		rect[3] = 0;
	}
}

void UI::GUIListView::EnsureVisible(UOSInt index)
{
	SendMessage((HWND)this->hwnd, LVM_ENSUREVISIBLE, index, (LPARAM)TRUE);
}

void UI::GUIListView::BeginUpdate()
{
	this->SetVisible(false);
}

void UI::GUIListView::EndUpdate()
{
	this->SetVisible(true);
}

Text::CString UI::GUIListView::GetObjectClass()
{
	return CSTR("ListView");
}

OSInt UI::GUIListView::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case NM_CUSTOMDRAW:
		break;
	case NM_SETFOCUS:
		break;
	case NM_KILLFOCUS:
		break;
#ifndef _WIN32_WCE
	case NM_RELEASEDCAPTURE:
		this->changing = true;
		break;
#endif
	case LVN_ITEMCHANGING:
		break;
	case LVN_ITEMCHANGED:
		if (!this->changing)
		{
			this->EventSelChg();
		}
		break;
	case LVN_DELETEALLITEMS:
	case LVN_INSERTITEM:
		break;
	case LVN_BEGINDRAG:
		this->EventSelChg();
		break;
	case NM_CLICK:
		this->changing = false;
		this->EventSelChg();
		break;
	case NM_DBLCLK:
#ifdef _WIN32_WCE
		{
			NMHDR *hdr;
			hdr = (NMHDR*)lParam;
			this->EventDblClk(hdr->idFrom);
		}
#else
		{
			NMITEMACTIVATE *ia;
			ia = (NMITEMACTIVATE*)lParam;
			this->EventDblClk((UInt32)ia->iItem);
		}
#endif
		break;
	default:
		lParam = 0;
		break;
	}
	return 0;
}

void UI::GUIListView::OnSizeChanged(Bool updateScn)
{
	if (this->lvStyle == UI::GUIListView::LVSTYLE_ICON)
	{
		SendMessage((HWND)this->hwnd, LVM_ARRANGE, LVA_DEFAULT, 0);
	}
	UOSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

void UI::GUIListView::EventSelChg()
{
	UOSInt i;
	i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIListView::EventDblClk(UOSInt itemIndex)
{
	UOSInt i;
	i = this->dblClkHdlrs->GetCount();
	while (i-- > 0)
	{
		this->dblClkHdlrs->GetItem(i)(this->dblClkObjs->GetItem(i), itemIndex);
	}
}

void UI::GUIListView::SetDPI(Double hdpi, Double ddpi)
{
	Double oldHDPI = this->hdpi;
	Double oldDDPI = this->ddpi;
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}
	UOSInt i = 0;
	OSInt ret;
	LVCOLUMNW col;
	col.mask = LVCF_WIDTH;

	Double ratio = this->hdpi / oldHDPI * oldDDPI / this->ddpi;
	while (i < this->colCnt)
	{
		ret = SendMessage((HWND)this->hwnd, LVM_GETCOLUMNW, i, (LPARAM)&col);
		if (ret != 0)
		{
			col.cx = Double2Int32(col.cx * ratio);
			ret = SendMessage((HWND)this->hwnd, LVM_SETCOLUMNW, i, (LPARAM)&col);
		}
		i++;
	}
}

void UI::GUIListView::HandleSelChg(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUIListView::HandleDblClk(ItemEvent hdlr, void *userObj)
{
	this->dblClkHdlrs->Add(hdlr);
	this->dblClkObjs->Add(userObj);
}
