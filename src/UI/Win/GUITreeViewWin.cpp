#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUITreeView.h"
#include "UI/Win/WinCore.h"
#include <windows.h>
#include <commctrl.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

UI::GUITreeView::TreeItem::TreeItem(AnyType itemObj, NN<Text::String> txt)
{
	this->hTreeItem = hTreeItem;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = txt->Clone();
}

UI::GUITreeView::TreeItem::TreeItem(AnyType itemObj, Text::CStringNN txt)
{
	this->hTreeItem = hTreeItem;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = Text::String::New(txt);
}

UI::GUITreeView::TreeItem::~TreeItem()
{
	NN<TreeItem> item;
	UOSInt i;
	i = this->children.GetCount();
	while (i-- > 0)
	{
		item = this->children.GetItemNoCheck(i);
		item.Delete();
	}
	this->txt->Release();
}

void UI::GUITreeView::TreeItem::AddChild(NN<UI::GUITreeView::TreeItem> child)
{
	this->children.Add(child);
	child->SetParent(this);
}
void UI::GUITreeView::TreeItem::SetParent(Optional<UI::GUITreeView::TreeItem> parent)
{
	this->parent = parent;
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::TreeItem::GetParent()
{
	return this->parent;
}

AnyType UI::GUITreeView::TreeItem::GetItemObj()
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

void UI::GUITreeView::TreeItem::SetText(Text::CStringNN txt)
{
	if (txt.leng == 0)
	{
		return;
	}
	this->txt->Release();
	this->txt = Text::String::New(txt);
}

NN<Text::String> UI::GUITreeView::TreeItem::GetText() const
{
	return this->txt;
}

UOSInt UI::GUITreeView::TreeItem::GetChildCount()
{
	return this->children.GetCount();
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::TreeItem::GetChild(UOSInt index)
{
	return this->children.GetItem(index);
}

OSInt __stdcall UI::GUITreeView::TVWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUITreeView *me = (UI::GUITreeView*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	NN<TreeItem> dragItem;
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
			if ((hitTarget = (HTREEITEM)SendMessage((HWND)me->hwnd.OrNull(), TVM_HITTEST, 0, (LPARAM)&tvht)) != 0)
				SendMessage((HWND)me->hwnd.OrNull(), TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hitTarget);

//			ImageList_DragShowNolock(TRUE); 
		}
		else if (me->autoFocus && me->IsFormFocused() && !me->editing)
		{
			SetFocus((HWND)hWnd);
		}
		break;
	case WM_LBUTTONUP:
		if (me->draging && dragItem.Set(me->dragItem))
		{
			HTREEITEM Selected;

//			ImageList_DragLeave((HWND)me->hwnd);
//			ImageList_EndDrag();
			Selected = (HTREEITEM)SendMessage((HWND)me->hwnd.OrNull(), TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
			SendMessage((HWND)me->hwnd.OrNull(), TVM_SELECTITEM, TVGN_CARET, (LPARAM)Selected);
			SendMessage((HWND)me->hwnd.OrNull(), TVM_SELECTITEM, TVGN_DROPHILITE, 0);
			me->ReleaseCapture();
			ShowCursor(TRUE); 
			me->draging = false;
			if (Selected != dragItem->GetHItem())
			{
				NN<TreeItem> dropItem;
				TVITEMW itm;
				itm.mask = TVIF_HANDLE | TVIF_PARAM;
				itm.hItem = Selected;
				itm.lParam = 0;
				SendMessage((HWND)me->hwnd.OrNull(), TVM_GETITEMW, 0, (LPARAM)&itm);
				if (dropItem.Set((TreeItem*)itm.lParam))
				{
					me->EventDragItem(dragItem, dropItem);
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
	NN<TreeItem> item;
	UOSInt i;
	i = this->treeItems.GetCount();
	while (i-- > 0)
	{
		item = this->treeItems.GetItemNoCheck(i);
		item.Delete();
	}
	this->treeItems.Clear();
}

UI::GUITreeView::GUITreeView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
	Math::Size2DDbl sz = parent->GetClientSize();
	UInt32 style = WS_TABSTOP | WS_CHILD | WS_VSCROLL | WS_BORDER | TVS_EDITLABELS;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, WC_TREEVIEWW, (const UTF8Char*)"TreeView", style, WS_EX_CLIENTEDGE, 0, 0, sz.x, sz.y);
	this->oriWndProc = (void*)UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)TVWndProc);
	this->autoFocus = false;
	this->editing = false;
	this->draging = false;
}

UI::GUITreeView::~GUITreeView()
{
	UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
	FreeItems();
}

void UI::GUITreeView::EventSelectionChange()
{
	UOSInt i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
	}
}

void UI::GUITreeView::EventDoubleClick()
{
}

void UI::GUITreeView::EventRightClicked()
{
	UOSInt i = this->rightClkHdlrs.GetCount();
	while (i-- > 0)
	{
		this->rightClkHdlrs.GetItem(i)(this->rightClkObjs.GetItem(i));
	}
}

OSInt UI::GUITreeView::EventBeginLabelEdit(NN<TreeItem> item)
{
	return 0;
}

OSInt UI::GUITreeView::EventEndLabelEdit(NN<TreeItem> item, UnsafeArray<const UTF8Char> newLabel)
{
	return 1;
}

void UI::GUITreeView::EventDragItem(NN<TreeItem> dragItem, NN<TreeItem> dropItem)
{

}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::InsertItem(Optional<UI::GUITreeView::TreeItem> parent, Optional<UI::GUITreeView::TreeItem> insertAfter, NN<Text::String> itemText, AnyType itemObj)
{
	NN<TreeItem> item;
	NN<TreeItem> nnparent;
	TVINSERTSTRUCTW is;
	if (parent.SetTo(nnparent))
	{
		is.hParent = (HTREEITEM)nnparent->GetHItem();
	}
	else
	{
		is.hParent = TVI_ROOT;
	}
	if (insertAfter.SetTo(item))
	{
		is.hInsertAfter = (HTREEITEM)item->GetHItem();
	}
	else
	{
		is.hInsertAfter = TVI_LAST;
	}
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText->v);
	NEW_CLASSNN(item, TreeItem(itemObj, itemText));
	is.item.mask = TVIF_TEXT | TVIF_PARAM;
	is.item.lParam = (LPARAM)item.Ptr();
	is.item.cchTextMax = (Int32)Text::StrCharCnt(wptr);
	is.item.pszText = (LPWSTR)wptr.Ptr();
	HTREEITEM hItem = (HTREEITEM)SendMessage((HWND)hwnd.OrNull(), TVM_INSERTITEMW, 0, (LPARAM)&is);
	Text::StrDelNew(wptr);
	if(hItem == 0)
	{
		item.Delete();
		return 0;
	}
	item->SetHItem(hItem);

	if (parent.SetTo(nnparent))
	{
		nnparent->AddChild(item);
	}
	else
	{
		this->treeItems.Add(item);
	}
	return item;
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::InsertItem(Optional<UI::GUITreeView::TreeItem> parent, Optional<UI::GUITreeView::TreeItem> insertAfter, Text::CStringNN itemText, AnyType itemObj)
{
	NN<TreeItem> item;
	NN<TreeItem> nnparent;
	TVINSERTSTRUCTW is;
	if (parent.SetTo(nnparent))
	{
		is.hParent = (HTREEITEM)nnparent->GetHItem();
	}
	else
	{
		is.hParent = TVI_ROOT;
	}
	if (insertAfter.SetTo(item))
	{
		is.hInsertAfter = (HTREEITEM)item->GetHItem();
	}
	else
	{
		is.hInsertAfter = TVI_LAST;
	}
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText.v);
	NEW_CLASSNN(item, TreeItem(itemObj, itemText));
	is.item.mask = TVIF_TEXT | TVIF_PARAM;
	is.item.lParam = (LPARAM)item.Ptr();
	is.item.cchTextMax = (Int32)Text::StrCharCnt(wptr);
	is.item.pszText = (LPWSTR)wptr.Ptr();
	HTREEITEM hItem = (HTREEITEM)SendMessage((HWND)hwnd.OrNull(), TVM_INSERTITEMW, 0, (LPARAM)&is);
	Text::StrDelNew(wptr);
	if(hItem == 0)
	{
		item.Delete();
		return 0;
	}
	item->SetHItem(hItem);

	if (parent.SetTo(nnparent))
	{
		nnparent->AddChild(item);
	}
	else
	{
		this->treeItems.Add(item);
	}
	return item;
}

AnyType UI::GUITreeView::RemoveItem(NN<UI::GUITreeView::TreeItem> item)
{
	UOSInt i = this->treeItems.IndexOf(item);
	if (i != INVALID_INDEX)
	{
		AnyType obj = item->GetItemObj();
		SendMessage((HWND)hwnd.OrNull(), TVM_DELETEITEM, 0, (LPARAM)item->GetHItem());
		this->treeItems.RemoveAt(i);
		item.Delete();
		return obj;
	}
	else
	{
		return 0;
	}
}

void UI::GUITreeView::ClearItems()
{
	SendMessage((HWND)hwnd.OrNull(), TVM_DELETEITEM, 0, 0);
	FreeItems();
}

UOSInt UI::GUITreeView::GetRootCount()
{
	return this->treeItems.GetCount();
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::GetRootItem(UOSInt index)
{
	return this->treeItems.GetItem(index);
}

void UI::GUITreeView::ExpandItem(NN<UI::GUITreeView::TreeItem> titem)
{
	SendMessage((HWND)this->hwnd.OrNull(), TVM_EXPAND, TVE_EXPAND, (LPARAM)titem->GetHItem());
}

Bool UI::GUITreeView::IsExpanded(NN<UI::GUITreeView::TreeItem> titem)
{
	return (SendMessage((HWND)this->hwnd.OrNull(), TVM_GETITEMSTATE, (WPARAM)titem->GetHItem(), TVIS_EXPANDED) & TVIS_EXPANDED) != 0;
}

void UI::GUITreeView::SetHasLines(Bool hasLines)
{
	Int32 ws = GetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE);
	if (hasLines)
	{
		if ((ws & TVS_HASLINES) == 0)
		{
			ws |= TVS_HASLINES;
			SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd.OrNull(), 0, false);
		}
	}
	else
	{
		if (ws & TVS_HASLINES)
		{
			ws &= ~TVS_HASLINES;
			SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd.OrNull(), 0, false);
		}
	}
}

void UI::GUITreeView::SetHasCheckBox(Bool hasCheckBox)
{
	Int32 ws = GetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE);
	if (hasCheckBox)
	{
		if ((ws & TVS_CHECKBOXES) == 0)
		{
			ws |= TVS_CHECKBOXES;
			SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd.OrNull(), 0, false);
		}
	}
	else
	{
		if (ws & TVS_CHECKBOXES)
		{
			ws &= ~TVS_CHECKBOXES;
			SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd.OrNull(), 0, false);
		}
	}
}

void UI::GUITreeView::SetHasButtons(Bool hasButtons)
{
	Int32 ws = GetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE);
	if (hasButtons)
	{
		if ((ws & TVS_HASBUTTONS) == 0)
		{
			ws |= TVS_HASBUTTONS;
			SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd.OrNull(), 0, false);
		}
	}
	else
	{
		if (ws & TVS_HASBUTTONS)
		{
			ws &= ~TVS_HASBUTTONS;
			SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd.OrNull(), 0, false);
		}
	}
}

void UI::GUITreeView::SetAutoFocus(Bool autoFocus)
{
	this->autoFocus = autoFocus;
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::GetSelectedItem()
{
	HTREEITEM hTreeItem = (HTREEITEM)SendMessage((HWND)this->hwnd.OrNull(), TVM_GETNEXTITEM, TVGN_CARET, 0);
	if (hTreeItem == 0)
		return 0;
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_PARAM;
	item.hItem =  hTreeItem;
	item.lParam = 0;
	if (SendMessage((HWND)this->hwnd.OrNull(), TVM_GETITEM, 0, (LPARAM)&item))
	{
		return (UI::GUITreeView::TreeItem *)item.lParam;
	}
	return 0;
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::GetHighlightItem()
{
	HTREEITEM hTreeItem = (HTREEITEM)SendMessage((HWND)this->hwnd.OrNull(), TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
	if (hTreeItem == 0)
	{
		hTreeItem = (HTREEITEM)SendMessage((HWND)this->hwnd.OrNull(), TVM_GETNEXTITEM, TVGN_CARET, 0);
		if (hTreeItem == 0)
			return 0;
	}
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_PARAM;
	item.hItem =  hTreeItem;
	item.lParam = 0;
	if (SendMessage((HWND)this->hwnd.OrNull(), TVM_GETITEM, 0, (LPARAM)&item))
	{
		return (UI::GUITreeView::TreeItem *)item.lParam;
	}
	return 0;
}

void UI::GUITreeView::BeginEdit(NN<TreeItem> item)
{
	SendMessage((HWND)this->hwnd.OrNull(), TVM_SELECTITEM, TVGN_CARET, (LPARAM)item->GetHItem());
	SendMessage((HWND)this->hwnd.OrNull(), TVM_EDITLABEL, 0, (LPARAM)item->GetHItem());
}

Text::CStringNN UI::GUITreeView::GetObjectClass() const
{
	return CSTR("TreeView");
}

OSInt UI::GUITreeView::OnNotify(UInt32 code, void *lParam)
{
	NMTVDISPINFOW *info;
	LPNMTREEVIEW lpnmtv;
	OSInt retVal;
	NN<UI::GUITreeView::TreeItem> item;
	switch (code)
	{
	case NM_RCLICK:
		EventRightClicked();
		break;
	case TVN_BEGINLABELEDIT:
		info = (NMTVDISPINFOW*)lParam;
		if (item.Set((UI::GUITreeView::TreeItem*)info->item.lParam))
		{
			retVal = EventBeginLabelEdit(item);
			if (retVal == 0)
			{
				this->editing = true;
			}
		}
		else
		{
			retVal = 0;
		}
		return retVal;
	case TVN_ENDLABELEDIT:
		{
			this->editing = false;
			info = (NMTVDISPINFOW*)lParam;
			NN<Text::String> s = Text::String::NewNotNull(info->item.pszText);
			if (item.Set((UI::GUITreeView::TreeItem*)info->item.lParam))
			{
				retVal = EventEndLabelEdit(item, s->v);
				if (retVal != 0)
				{
					item->SetText(s->ToCString());
				}
			}
			else
			{
				retVal = 0;
			}
			s->Release();
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

void UI::GUITreeView::HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUITreeView::HandleRightClick(UI::UIEvent hdlr, AnyType userObj)
{
	this->rightClkHdlrs.Add(hdlr);
	this->rightClkObjs.Add(userObj);
}
