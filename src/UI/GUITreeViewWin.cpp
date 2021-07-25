#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/GUITreeView.h"
#include <windows.h>
#include <commctrl.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

UI::GUITreeView::TreeItem::TreeItem(void *itemObj, const UTF8Char *txt)
{
	NEW_CLASS(this->children, Data::ArrayList<UI::GUITreeView::TreeItem *>());
	this->hTreeItem = hTreeItem;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = Text::StrCopyNew(txt);
}

UI::GUITreeView::TreeItem::~TreeItem()
{
	TreeItem *item;
	UOSInt i;
	i = this->children->GetCount();
	while (i-- > 0)
	{
		item = this->children->GetItem(i);
		DEL_CLASS(item);
	}
	DEL_CLASS(this->children);
	if (this->txt)
	{
		Text::StrDelNew(this->txt);
	}
}

void UI::GUITreeView::TreeItem::AddChild(UI::GUITreeView::TreeItem *child)
{
	this->children->Add(child);
	child->SetParent(this);
}
void UI::GUITreeView::TreeItem::SetParent(UI::GUITreeView::TreeItem *parent)
{
	this->parent = parent;
}

UI::GUITreeView::TreeItem *UI::GUITreeView::TreeItem::GetParent()
{
	return this->parent;
}

void *UI::GUITreeView::TreeItem::GetItemObj()
{
	return this->itemObj;
}

void UI::GUITreeView::TreeItem::SetHItem(void *hTreeItem)
{
	this->hTreeItem = hTreeItem;
}

void *UI::GUITreeView::TreeItem::GetHItem()
{
	return this->hTreeItem;
}

void UI::GUITreeView::TreeItem::SetText(const UTF8Char *txt)
{
	if (txt == 0)
	{
		return;
	}
	if (this->txt)
	{
		Text::StrDelNew(this->txt);
	}
	this->txt = Text::StrCopyNew(txt);
}

const UTF8Char *UI::GUITreeView::TreeItem::GetText()
{
	return this->txt;
}

UOSInt UI::GUITreeView::TreeItem::GetChildCount()
{
	return this->children->GetCount();
}

UI::GUITreeView::TreeItem *UI::GUITreeView::TreeItem::GetChild(UOSInt index)
{
	return this->children->GetItem(index);
}

OSInt __stdcall UI::GUITreeView::TVWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUITreeView *me = (UI::GUITreeView*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_MOUSEMOVE:
		if (me->draging)
		{
			HTREEITEM hitTarget;
			TVHITTESTINFO tvht;
			POINTS Pos = MAKEPOINTS(lParam);
//			ImageList_DragMove(Pos.x - 32, Pos.y - 25);
//			ImageList_DragShowNolock(FALSE);
			tvht.pt.x = Pos.x;
			tvht.pt.y = Pos.y;
			if ((hitTarget = (HTREEITEM)SendMessage((HWND)me->hwnd, TVM_HITTEST, 0, (LPARAM)&tvht)) != 0)
				SendMessage((HWND)me->hwnd, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hitTarget);

//			ImageList_DragShowNolock(TRUE); 
		}
		else if (me->autoFocus && me->IsFormFocused() && !me->editing)
		{
			SetFocus((HWND)hWnd);
		}
		break;
	case WM_LBUTTONUP:
		if (me->draging)
		{
			HTREEITEM Selected;

//			ImageList_DragLeave((HWND)me->hwnd);
//			ImageList_EndDrag();
			Selected = (HTREEITEM)SendMessage((HWND)me->hwnd, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
			SendMessage((HWND)me->hwnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)Selected);
			SendMessage((HWND)me->hwnd, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
			me->ReleaseCapture();
			ShowCursor(TRUE); 
			me->draging = false;
			if (Selected != me->dragItem->GetHItem())
			{
				TVITEMW itm;
				itm.mask = TVIF_HANDLE | TVIF_PARAM;
				itm.hItem = Selected;
				itm.lParam = 0;
				SendMessage((HWND)me->hwnd, TVM_GETITEMW, 0, (LPARAM)&itm);
				if (itm.lParam)
				{
					me->EventDragItem(me->dragItem, (TreeItem*)itm.lParam);
				}
			}
		}
		break;
	default:
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

void UI::GUITreeView::FreeItems()
{
	TreeItem *item;
	UOSInt i;
	i = this->treeItems->GetCount();
	while (i-- > 0)
	{
		item = this->treeItems->RemoveAt(i);
		DEL_CLASS(item);
	}
}

UI::GUITreeView::GUITreeView(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	Double w;
	Double h;
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->rightClkHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->rightClkObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->treeItems, Data::ArrayList<TreeItem*>());
	parent->GetClientSize(&w, &h);
	UInt32 style = WS_TABSTOP | WS_CHILD | WS_VSCROLL | WS_BORDER | TVS_EDITLABELS;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, WC_TREEVIEWW, (const UTF8Char*)"TreeView", style, WS_EX_CLIENTEDGE, 0, 0, w, h);
	this->oriWndProc = (void*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)TVWndProc);
	this->autoFocus = false;
	this->editing = false;
	this->draging = false;
}

UI::GUITreeView::~GUITreeView()
{
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
	FreeItems();
	DEL_CLASS(this->treeItems);
	DEL_CLASS(this->rightClkHdlrs);
	DEL_CLASS(this->rightClkObjs);
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
}

void UI::GUITreeView::EventSelectionChange()
{
	UOSInt i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUITreeView::EventDoubleClick()
{
}

void UI::GUITreeView::EventRightClicked()
{
	UOSInt i = this->rightClkHdlrs->GetCount();
	while (i-- > 0)
	{
		this->rightClkHdlrs->GetItem(i)(this->rightClkObjs->GetItem(i));
	}
}

OSInt UI::GUITreeView::EventBeginLabelEdit(TreeItem *item)
{
	return 0;
}

OSInt UI::GUITreeView::EventEndLabelEdit(TreeItem *item, const UTF8Char *newLabel)
{
	return 1;
}

void UI::GUITreeView::EventDragItem(TreeItem *dragItem, TreeItem *dropItem)
{

}

UI::GUITreeView::TreeItem *UI::GUITreeView::InsertItem(UI::GUITreeView::TreeItem *parent, UI::GUITreeView::TreeItem *insertAfter, const UTF8Char *itemText, void *itemObj)
{
	TreeItem *item;
	TVINSERTSTRUCTW is;
	if (parent)
	{
		is.hParent = (HTREEITEM)parent->GetHItem();
	}
	else
	{
		is.hParent = TVI_ROOT;
	}
	if (insertAfter)
	{
		is.hInsertAfter = (HTREEITEM)insertAfter->GetHItem();
	}
	else
	{
		is.hInsertAfter = TVI_LAST;
	}
	const WChar *wptr = Text::StrToWCharNew(itemText);
	NEW_CLASS(item, TreeItem(itemObj, itemText));
	is.item.mask = TVIF_TEXT | TVIF_PARAM;
	is.item.lParam = (LPARAM)item;
	is.item.cchTextMax = (Int32)Text::StrCharCnt(wptr);
	is.item.pszText = (LPWSTR)wptr;
	HTREEITEM hItem = (HTREEITEM)SendMessage((HWND)hwnd, TVM_INSERTITEMW, 0, (LPARAM)&is);
	Text::StrDelNew(wptr);
	if(hItem == 0)
	{
		DEL_CLASS(item);
		return 0;
	}
	item->SetHItem(hItem);

	if (parent)
	{
		parent->AddChild(item);
	}
	else
	{
		this->treeItems->Add(item);
	}
	return item;
}

void *UI::GUITreeView::RemoveItem(UI::GUITreeView::TreeItem *item)
{
	UOSInt i = this->treeItems->IndexOf(item);
	if (i != INVALID_INDEX)
	{
		void *obj = item->GetItemObj();
		SendMessage((HWND)hwnd, TVM_DELETEITEM, 0, (LPARAM)item->GetHItem());
		this->treeItems->RemoveAt(i);
		DEL_CLASS(item);
		return obj;
	}
	else
	{
		return 0;
	}
}

void UI::GUITreeView::ClearItems()
{
	SendMessage((HWND)hwnd, TVM_DELETEITEM, 0, 0);
	FreeItems();
}

UOSInt UI::GUITreeView::GetRootCount()
{
	return this->treeItems->GetCount();
}

UI::GUITreeView::TreeItem *UI::GUITreeView::GetRootItem(UOSInt index)
{
	return this->treeItems->GetItem(index);
}

void UI::GUITreeView::ExpandItem(UI::GUITreeView::TreeItem *titem)
{
	SendMessage((HWND)this->hwnd, TVM_EXPAND, TVE_EXPAND, (LPARAM)titem->GetHItem());
}

Bool UI::GUITreeView::IsExpanded(UI::GUITreeView::TreeItem *titem)
{
	return (SendMessage((HWND)this->hwnd, TVM_GETITEMSTATE, (WPARAM)titem->GetHItem(), TVIS_EXPANDED) & TVIS_EXPANDED) != 0;
}

void UI::GUITreeView::SetHasLines(Bool hasLines)
{
	Int32 ws = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	if (hasLines)
	{
		if ((ws & TVS_HASLINES) == 0)
		{
			ws |= TVS_HASLINES;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
	else
	{
		if (ws & TVS_HASLINES)
		{
			ws &= ~TVS_HASLINES;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
}

void UI::GUITreeView::SetHasCheckBox(Bool hasCheckBox)
{
	Int32 ws = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	if (hasCheckBox)
	{
		if ((ws & TVS_CHECKBOXES) == 0)
		{
			ws |= TVS_CHECKBOXES;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
	else
	{
		if (ws & TVS_CHECKBOXES)
		{
			ws &= ~TVS_CHECKBOXES;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
}

void UI::GUITreeView::SetHasButtons(Bool hasButtons)
{
	Int32 ws = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	if (hasButtons)
	{
		if ((ws & TVS_HASBUTTONS) == 0)
		{
			ws |= TVS_HASBUTTONS;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
	else
	{
		if (ws & TVS_HASBUTTONS)
		{
			ws &= ~TVS_HASBUTTONS;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
}

void UI::GUITreeView::SetAutoFocus(Bool autoFocus)
{
	this->autoFocus = autoFocus;
}

UI::GUITreeView::TreeItem *UI::GUITreeView::GetSelectedItem()
{
	HTREEITEM hTreeItem = (HTREEITEM)SendMessage((HWND)this->hwnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
	if (hTreeItem == 0)
		return 0;
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_PARAM;
	item.hItem =  hTreeItem;
	item.lParam = 0;
	if (SendMessage((HWND)this->hwnd, TVM_GETITEM, 0, (LPARAM)&item))
	{
		return (UI::GUITreeView::TreeItem *)item.lParam;
	}
	return 0;
}

UI::GUITreeView::TreeItem *UI::GUITreeView::GetHighlightItem()
{
	HTREEITEM hTreeItem = (HTREEITEM)SendMessage((HWND)this->hwnd, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
	if (hTreeItem == 0)
	{
		hTreeItem = (HTREEITEM)SendMessage((HWND)this->hwnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
		if (hTreeItem == 0)
			return 0;
	}
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_PARAM;
	item.hItem =  hTreeItem;
	item.lParam = 0;
	if (SendMessage((HWND)this->hwnd, TVM_GETITEM, 0, (LPARAM)&item))
	{
		return (UI::GUITreeView::TreeItem *)item.lParam;
	}
	return 0;
}

void UI::GUITreeView::BeginEdit(TreeItem *item)
{
	SendMessage((HWND)this->hwnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item->GetHItem());
	SendMessage((HWND)this->hwnd, TVM_EDITLABEL, 0, (LPARAM)item->GetHItem());
}

const UTF8Char *UI::GUITreeView::GetObjectClass()
{
	return (const UTF8Char*)"TreeView";
}

OSInt UI::GUITreeView::OnNotify(UInt32 code, void *lParam)
{
	NMTVDISPINFOW *info;
	LPNMTREEVIEW lpnmtv;
	OSInt retVal;
	UI::GUITreeView::TreeItem *item;
	switch (code)
	{
	case NM_RCLICK:
		EventRightClicked();
		break;
	case TVN_BEGINLABELEDIT:
		info = (NMTVDISPINFOW*)lParam;
		retVal = EventBeginLabelEdit((UI::GUITreeView::TreeItem*)info->item.lParam);
		if (retVal == 0)
		{
			this->editing = true;
		}
		return retVal;
	case TVN_ENDLABELEDIT:
		{
			this->editing = false;
			info = (NMTVDISPINFOW*)lParam;
			const UTF8Char *u8ptr = Text::StrToUTF8New(info->item.pszText);
			retVal = EventEndLabelEdit((UI::GUITreeView::TreeItem*)info->item.lParam, u8ptr);
			if (retVal != 0)
			{
				item = (UI::GUITreeView::TreeItem*)info->item.lParam;
				item->SetText(u8ptr);
			}
			Text::StrDelNew(u8ptr);
		}
		return retVal;
	case TVN_BEGINDRAG:
		//http://msdn.microsoft.com/en-us/library/windows/desktop/bb773504%28v=vs.85%29.aspx
		//http://www.codeproject.com/Articles/3448/Using-TreeControl-TreeView-under-Win32-API
		lpnmtv = (LPNMTREEVIEW) lParam;
		this->dragItem = (TreeItem*)lpnmtv->itemNew.lParam;
//		himgDrag = (void*)SendMessage((HWND)this->hwnd, TVM_CREATEDRAGIMAGE, 0, (LPARAM)lpnmtv->itemNew.hItem);
//		ImageList_BeginDrag((HIMAGELIST)himgDrag, 0, 0, 0);
//		ImageList_DragEnter((HWND)this->hwnd, lpnmtv->ptDrag.x, lpnmtv->ptDrag.y);
//		ShowCursor(FALSE); // no need mouse cursor
      	this->SetCapture();
		this->draging = true;
		return 0;
//	case TVN_ITEMCHANGED:
//		InvalidateRect((HWND)this->hwnd, 0, false);

	case TVN_SELCHANGED:
		this->EventSelectionChange();
		return 0;
	case NM_DBLCLK:
		this->EventDoubleClick();
		return 0;
	case 0:
	default:
		break;
	}
	return 0;
}

void UI::GUITreeView::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUITreeView::HandleRightClick(UI::UIEvent hdlr, void *userObj)
{
	this->rightClkHdlrs->Add(hdlr);
	this->rightClkObjs->Add(userObj);
}
