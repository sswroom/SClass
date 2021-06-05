#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkWidget *lbl;
	const UTF8Char *txt;
} PageInfo;

void GUITabControl_SelChange(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data)
{
	UI::GUITabControl *me = (UI::GUITabControl*)user_data;
	me->EventSelChange(page_num);
}

gboolean GUITabControl_OnShow(gpointer user_data)
{
	UI::GUITabControl *me = (UI::GUITabControl*)user_data;
	me->OnSizeChanged(false);
	return false;
}

UI::GUITabControl::GUITabControl(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->tabPages, Data::ArrayList<UI::GUITabPage*>());
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void *>());
	this->selIndex = 0;
	this->hwnd = gtk_notebook_new();
	g_signal_connect((GtkWidget*)this->hwnd, "switch-page", G_CALLBACK(GUITabControl_SelChange), this);
	parent->AddChild(this);
	this->Show();
	g_idle_add(GUITabControl_OnShow, this);
}

UI::GUITabControl::~GUITabControl()
{
	UI::GUITabPage *tp;
	PageInfo *page;
	OSInt i = this->tabPages->GetCount();
	while (i-- > 0)
	{
		tp = this->tabPages->GetItem(i);
		page = (PageInfo*)tp->GetCustObj();
		Text::StrDelNew(page->txt);
		MemFree(page);
		DEL_CLASS(tp);
	}
	DEL_CLASS(this->tabPages);
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
}

UI::GUITabPage *UI::GUITabControl::AddTabPage(const UTF8Char *tabName)
{
	UI::GUITabPage *tp;
	PageInfo *page;
	NEW_CLASS(tp, UI::GUITabPage(this->ui, 0, this, this->tabPages->GetCount()));
	page = MemAlloc(PageInfo, 1);
	page->lbl = gtk_label_new((const Char*)tabName);
	page->txt = Text::StrCopyNew(tabName);
	tp->SetCustObj(page);
	gtk_notebook_append_page((GtkNotebook*)this->hwnd, (GtkWidget*)tp->GetHandle(), page->lbl);
	OSInt x;
	OSInt y;
	OSInt w;
	OSInt h;
	this->GetTabPageRect(&x, &y, &w, &h);
	tp->SetRect(0, 0, w, h, false);
	tp->SetDPI(this->hdpi, this->ddpi);
	tp->Show();
	this->tabPages->Add(tp);
	return tp;
}

void UI::GUITabControl::SetSelectedIndex(UOSInt index)
{
	if (this->selIndex != index)
	{
		gtk_notebook_set_current_page((GtkNotebook*)this->hwnd, index);
		this->EventSelChange(index);
	}
}

void UI::GUITabControl::SetSelectedPage(UI::GUITabPage *page)
{
	if (page == 0)
		return;
	UOSInt i = this->tabPages->GetCount();
	while (i-- > 0)
	{
		if (page == this->tabPages->GetItem(i))
		{
			SetSelectedIndex(i);
		}
	}
}

UOSInt UI::GUITabControl::GetSelectedIndex()
{
	return this->selIndex;
}

void UI::GUITabControl::SetTabPageName(UOSInt index, const UTF8Char *name)
{
	UI::GUITabPage *tp = this->tabPages->GetItem(index);
	if (tp == 0)
		return;
	PageInfo *page = (PageInfo*)tp->GetCustObj();
	gtk_label_set_text((GtkLabel*)page->lbl, (const Char*)name);
	Text::StrDelNew(page->txt);
	page->txt = Text::StrCopyNew(name);
}

UTF8Char *UI::GUITabControl::GetTabPageName(UOSInt index, UTF8Char *buff)
{
	UI::GUITabPage *tp = this->tabPages->GetItem(index);
	if (tp == 0)
		return 0;
	PageInfo *page = (PageInfo*)tp->GetCustObj();
	return Text::StrConcat(buff, page->txt);
}

void UI::GUITabControl::GetTabPageRect(OSInt *x, OSInt *y, OSInt *w, OSInt *h)
{
	UOSInt width;
	UOSInt height;
	OSInt btnH = 0;
	GtkAllocation clip;
	this->GetSizeP(&width, &height);
	GtkWidget *page;
	gtk_widget_get_allocation((GtkWidget*)this->hwnd, &clip);
	if (clip.y <= 0)
	{
		page = gtk_notebook_get_nth_page((GtkNotebook*)this->hwnd, -1);
		if (page)
		{
			gtk_widget_get_allocation(page, &clip);
		}
	}
	btnH = clip.y;
	if (x)
		*x = 0;
	if (y)
		*y = btnH;
	if (w)
		*w = width - 2;
	if (h)
		*h = height - btnH - 2;
}

void *UI::GUITabControl::GetTabPageFont()
{
	return this->GetFont();
}

const UTF8Char *UI::GUITabControl::GetObjectClass()
{
	return (const UTF8Char*)"TabControl";
}

OSInt UI::GUITabControl::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUITabControl::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}

	UOSInt w1;
	UOSInt h1;
	OSInt w2;
	OSInt h2;
	GetSizeP(&w1, &h1);
	GetTabPageRect(0, 0, &w2, &h2);
	i = this->tabPages->GetCount();
	while (i-- > 0)
	{
		this->tabPages->GetItem(i)->SetSizeP(w2, h2);
		this->tabPages->GetItem(i)->UpdateChildrenSize(false);
	}
}

void UI::GUITabControl::HandleSelChanged(UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUITabControl::SetDPI(Double hdpi, Double ddpi)
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

	UOSInt i = this->tabPages->GetCount();
	while (i-- > 0)
	{
		this->tabPages->GetItem(i)->SetDPI(hdpi, ddpi);
	}

	i = this->tabPages->GetCount();
	while (i-- > 0)
	{
		this->tabPages->GetItem(i)->UpdateChildrenSize(false);
	}
}

void UI::GUITabControl::EventSelChange(UOSInt index)
{
	if (this->selIndex != index)
	{
		this->selIndex = index;
		OSInt i;
		i = this->selChgHdlrs->GetCount();
		while (i-- > 0)
		{
			this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
		}
	}
}
