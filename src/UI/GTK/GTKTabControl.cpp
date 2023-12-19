#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "UI/GUITabPage.h"
#include "UI/GTK/GTKTabControl.h"

typedef struct
{
	GtkWidget *lbl;
	NotNullPtr<Text::String> txt;
} PageInfo;

void UI::GTK::GTKTabControl::SignalSwitchPage(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data)
{
	UI::GTK::GTKTabControl *me = (UI::GTK::GTKTabControl*)user_data;
	if (me->selIndex != page_num)
	{
		me->selIndex = page_num;
		me->EventSelChange();
	}
}

gboolean UI::GTK::GTKTabControl::SignalShown(gpointer user_data)
{
	UI::GTK::GTKTabControl *me = (UI::GTK::GTKTabControl*)user_data;
	me->OnSizeChanged(false);
	return false;
}

UI::GTK::GTKTabControl::GTKTabControl(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUITabControl(ui, parent)
{
	this->selIndex = 0;
	this->hwnd = (ControlHandle*)gtk_notebook_new();
	g_signal_connect((GtkWidget*)this->hwnd, "switch-page", G_CALLBACK(SignalSwitchPage), this);
	parent->AddChild(this);
	this->Show();
	g_idle_add(SignalShown, this);
}

UI::GTK::GTKTabControl::~GTKTabControl()
{
	NotNullPtr<UI::GUITabPage> tp;
	PageInfo *page;
	Data::ArrayIterator<NotNullPtr<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		tp = it.Next();
		page = (PageInfo*)tp->GetCustObj();
		page->txt->Release();
		MemFree(page);
		tp.Delete();
	}
}

NotNullPtr<UI::GUITabPage> UI::GTK::GTKTabControl::AddTabPage(NotNullPtr<Text::String> tabName)
{
	NotNullPtr<UI::GUITabPage> tp;
	PageInfo *page;
	NEW_CLASSNN(tp, UI::GUITabPage(this->ui, 0, *this, this->tabPages.GetCount()));
	page = MemAlloc(PageInfo, 1);
	page->lbl = gtk_label_new((const Char*)tabName->v);
	page->txt = tabName->Clone();
	tp->SetCustObj(page);
	gtk_notebook_append_page((GtkNotebook*)this->hwnd, (GtkWidget*)tp->GetHandle(), page->lbl);
	Math::RectArea<OSInt> rect = this->GetTabPageRect();
	tp->SetRect(0, 0, OSInt2Double(rect.GetWidth()), OSInt2Double(rect.GetHeight()), false);
	tp->SetDPI(this->hdpi, this->ddpi);
	tp->Show();
	this->tabPages.Add(tp);
	return tp;
}

NotNullPtr<UI::GUITabPage> UI::GTK::GTKTabControl::AddTabPage(Text::CStringNN tabName)
{
	NotNullPtr<UI::GUITabPage> tp;
	PageInfo *page;
	NEW_CLASSNN(tp, UI::GUITabPage(this->ui, 0, *this, this->tabPages.GetCount()));
	page = MemAlloc(PageInfo, 1);
	page->lbl = gtk_label_new((const Char*)tabName.v);
	page->txt = Text::String::New(tabName);
	tp->SetCustObj(page);
	gtk_notebook_append_page((GtkNotebook*)this->hwnd, (GtkWidget*)tp->GetHandle(), page->lbl);
	Math::RectArea<OSInt> rect = this->GetTabPageRect();
	tp->SetRect(0, 0, OSInt2Double(rect.GetWidth()), OSInt2Double(rect.GetHeight()), false);
	tp->SetDPI(this->hdpi, this->ddpi);
	tp->Show();
	this->tabPages.Add(tp);
	return tp;
}

void UI::GTK::GTKTabControl::SetSelectedIndex(UOSInt index)
{
	if (this->selIndex != index)
	{
		gtk_notebook_set_current_page((GtkNotebook*)this->hwnd, (gint)(OSInt)index);
		this->selIndex = index;
		this->EventSelChange();
	}
}

void UI::GTK::GTKTabControl::SetSelectedPage(NotNullPtr<UI::GUITabPage> page)
{
	UOSInt i = this->tabPages.GetCount();
	while (i-- > 0)
	{
		if (page.Ptr() == this->tabPages.GetItem(i).OrNull())
		{
			SetSelectedIndex(i);
		}
	}
}

UOSInt UI::GTK::GTKTabControl::GetSelectedIndex()
{
	return this->selIndex;
}

Optional<UI::GUITabPage> UI::GTK::GTKTabControl::GetSelectedPage()
{
	return this->tabPages.GetItem(this->selIndex);
}

void UI::GTK::GTKTabControl::SetTabPageName(UOSInt index, Text::CStringNN name)
{
	NotNullPtr<UI::GUITabPage> tp;
	if (!this->tabPages.GetItem(index).SetTo(tp))
		return;
	PageInfo *page = (PageInfo*)tp->GetCustObj();
	gtk_label_set_text((GtkLabel*)page->lbl, (const Char*)name.v);
	page->txt->Release();
	page->txt = Text::String::New(name);
}

UTF8Char *UI::GTK::GTKTabControl::GetTabPageName(UOSInt index, UTF8Char *buff)
{
	NotNullPtr<UI::GUITabPage> tp;
	if (!this->tabPages.GetItem(index).SetTo(tp))
		return 0;
	PageInfo *page = (PageInfo*)tp->GetCustObj();
	return page->txt->ConcatTo(buff);
}

Math::RectArea<OSInt> UI::GTK::GTKTabControl::GetTabPageRect()
{
	Math::Size2D<UOSInt> sz;
	OSInt btnH = 0;
	GtkAllocation clip;
	sz = this->GetSizeP();
	GtkWidget *page;
	clip.y = 0;
	clip.height = 0;

	page = gtk_notebook_get_nth_page((GtkNotebook*)this->hwnd, -1);
	if (page)
	{
//		printf("Page allocation\r\n");
		gtk_widget_get_allocation(page, &clip);
	}
	if (clip.y <= 0)
	{
		btnH = 16;		
	}
	else
	{
		Math::Coord2D<OSInt> pos = this->GetPositionP();
//		printf("TabPage clip.y = %d, clip.height = %d, height = %d, posY = %d\r\n", clip.y, clip.height, (UInt32)height, (Int32)posY);
		btnH = clip.y - pos.y;
	}
	return Math::RectArea<OSInt>(0, btnH, (OSInt)sz.x - 2, (OSInt)sz.y - btnH - 2);
}

OSInt UI::GTK::GTKTabControl::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKTabControl::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
	}

	Math::Size2D<UOSInt> sz1;
	Math::Size2D<OSInt> sz2;
	//sz1 = GetSizeP();
	sz2 = GetTabPageRect().GetSize();
	sz1 = Math::Size2D<UOSInt>((UOSInt)sz2.x, (UOSInt)sz2.y);
	NotNullPtr<GUITabPage> tp;
	Data::ArrayIterator<NotNullPtr<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		tp = it.Next();
		tp->SetSizeP(sz1);
		tp->UpdateChildrenSize(false);
	}
}

void UI::GTK::GTKTabControl::SetDPI(Double hdpi, Double ddpi)
{
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

	Data::ArrayIterator<NotNullPtr<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetDPI(hdpi, ddpi);
	}

	it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->UpdateChildrenSize(false);
	}
}
