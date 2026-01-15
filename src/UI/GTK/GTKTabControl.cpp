#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "UI/GUITabPage.h"
#include "UI/GTK/GTKTabControl.h"

typedef struct
{
	GtkWidget *lbl;
	NN<Text::String> txt;
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

UI::GTK::GTKTabControl::GTKTabControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUITabControl(ui, parent)
{
	this->selIndex = 0;
	this->hwnd = (ControlHandle*)gtk_notebook_new();
	g_signal_connect((GtkWidget*)this->hwnd.OrNull(), "switch-page", G_CALLBACK(SignalSwitchPage), this);
	parent->AddChild(*this);
	this->Show();
	g_idle_add(SignalShown, this);
}

UI::GTK::GTKTabControl::~GTKTabControl()
{
	NN<UI::GUITabPage> tp;
	PageInfo *page;
	Data::ArrayIterator<NN<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		tp = it.Next();
		page = (PageInfo*)tp->GetCustObj();
		page->txt->Release();
		MemFree(page);
		tp.Delete();
	}
}

NN<UI::GUITabPage> UI::GTK::GTKTabControl::AddTabPage(NN<Text::String> tabName)
{
	NN<UI::GUITabPage> tp;
	PageInfo *page;
	NEW_CLASSNN(tp, UI::GUITabPage(this->ui, nullptr, *this, this->tabPages.GetCount()));
	page = MemAlloc(PageInfo, 1);
	page->lbl = gtk_label_new((const Char*)tabName->v.Ptr());
	page->txt = tabName->Clone();
	tp->SetCustObj(page);
	gtk_notebook_append_page((GtkNotebook*)this->hwnd.OrNull(), (GtkWidget*)tp->GetHandle().OrNull(), page->lbl);
	Math::RectArea<IntOS> rect = this->GetTabPageRect();
	tp->SetRect(0, 0, IntOS2Double(rect.GetWidth()), IntOS2Double(rect.GetHeight()), false);
	tp->SetDPI(this->hdpi, this->ddpi);
	tp->Show();
	this->tabPages.Add(tp);
	return tp;
}

NN<UI::GUITabPage> UI::GTK::GTKTabControl::AddTabPage(Text::CStringNN tabName)
{
	NN<UI::GUITabPage> tp;
	PageInfo *page;
	NEW_CLASSNN(tp, UI::GUITabPage(this->ui, nullptr, *this, this->tabPages.GetCount()));
	page = MemAlloc(PageInfo, 1);
	page->lbl = gtk_label_new((const Char*)tabName.v.Ptr());
	page->txt = Text::String::New(tabName);
	tp->SetCustObj(page);
	gtk_notebook_append_page((GtkNotebook*)this->hwnd.OrNull(), (GtkWidget*)tp->GetHandle().OrNull(), page->lbl);
	Math::RectArea<IntOS> rect = this->GetTabPageRect();
	tp->SetRect(0, 0, IntOS2Double(rect.GetWidth()), IntOS2Double(rect.GetHeight()), false);
	tp->SetDPI(this->hdpi, this->ddpi);
	tp->Show();
	this->tabPages.Add(tp);
	return tp;
}

void UI::GTK::GTKTabControl::SetSelectedIndex(UIntOS index)
{
	if (this->selIndex != index)
	{
		gtk_notebook_set_current_page((GtkNotebook*)this->hwnd.OrNull(), (gint)(IntOS)index);
		this->selIndex = index;
		this->EventSelChange();
	}
}

void UI::GTK::GTKTabControl::SetSelectedPage(NN<UI::GUITabPage> page)
{
	UIntOS i = this->tabPages.GetCount();
	while (i-- > 0)
	{
		if (page.Ptr() == this->tabPages.GetItem(i).OrNull())
		{
			SetSelectedIndex(i);
		}
	}
}

UIntOS UI::GTK::GTKTabControl::GetSelectedIndex()
{
	return this->selIndex;
}

Optional<UI::GUITabPage> UI::GTK::GTKTabControl::GetSelectedPage()
{
	return this->tabPages.GetItem(this->selIndex);
}

void UI::GTK::GTKTabControl::SetTabPageName(UIntOS index, Text::CStringNN name)
{
	NN<UI::GUITabPage> tp;
	if (!this->tabPages.GetItem(index).SetTo(tp))
		return;
	PageInfo *page = (PageInfo*)tp->GetCustObj();
	gtk_label_set_text((GtkLabel*)page->lbl, (const Char*)name.v.Ptr());
	page->txt->Release();
	page->txt = Text::String::New(name);
}

UnsafeArrayOpt<UTF8Char> UI::GTK::GTKTabControl::GetTabPageName(UIntOS index, UnsafeArray<UTF8Char> buff)
{
	NN<UI::GUITabPage> tp;
	if (!this->tabPages.GetItem(index).SetTo(tp))
		return nullptr;
	PageInfo *page = (PageInfo*)tp->GetCustObj();
	return page->txt->ConcatTo(buff);
}

Math::RectArea<IntOS> UI::GTK::GTKTabControl::GetTabPageRect()
{
	Math::Size2D<UIntOS> sz;
	IntOS btnH = 0;
	GtkAllocation clip;
	sz = this->GetSizeP();
	GtkWidget *page;
	clip.y = 0;
	clip.height = 0;

	page = gtk_notebook_get_nth_page((GtkNotebook*)this->hwnd.OrNull(), -1);
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
		Math::Coord2D<IntOS> pos = this->GetPositionP();
//		printf("TabPage clip.y = %d, clip.height = %d, height = %d, posY = %d\r\n", clip.y, clip.height, (UInt32)height, (Int32)posY);
		btnH = clip.y - pos.y;
	}
	return Math::RectArea<IntOS>(0, btnH, (IntOS)sz.x - 2, (IntOS)sz.y - btnH - 2);
}

IntOS UI::GTK::GTKTabControl::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKTabControl::OnSizeChanged(Bool updateScn)
{
	UIntOS i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}

	Math::Size2D<UIntOS> sz1;
	Math::Size2D<IntOS> sz2;
	//sz1 = GetSizeP();
	sz2 = GetTabPageRect().GetSize();
	sz1 = Math::Size2D<UIntOS>((UIntOS)sz2.x, (UIntOS)sz2.y);
	NN<GUITabPage> tp;
	Data::ArrayIterator<NN<GUITabPage>> it = this->tabPages.Iterator();
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

	Data::ArrayIterator<NN<GUITabPage>> it = this->tabPages.Iterator();
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
