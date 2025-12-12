#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "UI/GUITabPage.h"
#include "UI/JavaUI/JUITabControl.h"

UI::JavaUI::JUITabControl::JUITabControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUITabControl(ui, parent)
{
	this->selIndex = 0;
}

UI::JavaUI::JUITabControl::~JUITabControl()
{
	this->tabPages.DeleteAll();
}

NN<UI::GUITabPage> UI::JavaUI::JUITabControl::AddTabPage(NN<Text::String> tabName)
{
	NN<UI::GUITabPage> tp;
	NEW_CLASSNN(tp, UI::GUITabPage(this->ui, 0, *this, this->tabPages.GetCount()));
	this->tabPages.Add(tp);
	return tp;
}

NN<UI::GUITabPage> UI::JavaUI::JUITabControl::AddTabPage(Text::CStringNN tabName)
{
	NN<UI::GUITabPage> tp;
	NEW_CLASSNN(tp, UI::GUITabPage(this->ui, 0, *this, this->tabPages.GetCount()));
	this->tabPages.Add(tp);
	return tp;
}

void UI::JavaUI::JUITabControl::SetSelectedIndex(UOSInt index)
{
	if (this->selIndex != index)
	{
		this->selIndex = index;
		this->EventSelChange();
	}
}

void UI::JavaUI::JUITabControl::SetSelectedPage(NN<UI::GUITabPage> page)
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

UOSInt UI::JavaUI::JUITabControl::GetSelectedIndex()
{
	return this->selIndex;
}

Optional<UI::GUITabPage> UI::JavaUI::JUITabControl::GetSelectedPage()
{
	return this->tabPages.GetItem(this->selIndex);
}

void UI::JavaUI::JUITabControl::SetTabPageName(UOSInt index, Text::CStringNN name)
{
	NN<UI::GUITabPage> tp;
	if (!this->tabPages.GetItem(index).SetTo(tp))
		return;
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JUITabControl::GetTabPageName(UOSInt index, UnsafeArray<UTF8Char> buff)
{
	NN<UI::GUITabPage> tp;
	if (!this->tabPages.GetItem(index).SetTo(tp))
		return 0;
	return 0;
}

Math::RectArea<OSInt> UI::JavaUI::JUITabControl::GetTabPageRect()
{
	return Math::RectArea<OSInt>(0, 0, 0, 0);
}

OSInt UI::JavaUI::JUITabControl::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUITabControl::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> evt = this->resizeHandlers.GetItem(i);
		evt.func(evt.userObj);
	}
}

void UI::JavaUI::JUITabControl::SetDPI(Double hdpi, Double ddpi)
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
