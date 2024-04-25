#include "Stdafx.h"
#include "UI/GUIComboBox.h"

UI::GUIComboBox::GUIComboBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
	this->autoComplete = false;
	this->nonUIEvent = false;
	this->lastTextLeng = 0;
}

UI::GUIComboBox::~GUIComboBox()
{
	this->itemTexts.FreeAll();
}

void UI::GUIComboBox::EventSelectionChange()
{
	UOSInt i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
	}
}

void UI::GUIComboBox::EventTextChanged()
{
	if (this->autoComplete || !this->nonUIEvent)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(sb);
		if (sb.GetLength() > this->lastTextLeng)
		{
			this->nonUIEvent = true;
			Data::ArrayIterator<NN<Text::String>> it = this->itemTexts.Iterator();
			NN<Text::String> s;
			while (it.HasNext())
			{
				s = it.Next();
				if (s->StartsWith(sb.ToString(), sb.GetLength()))
				{
					this->SetText(s->ToCString());
					//this->SetSelectedIndex(i);
					this->SetTextSelection(sb.GetLength(), s->leng);
					break;
				}
			}
			this->nonUIEvent = false;
		}
		this->lastTextLeng = sb.GetLength();
	}
}

UTF8Char *UI::GUIComboBox::GetSelectedItemText(UTF8Char *sbuff)
{
	UOSInt i = this->GetSelectedIndex();
	NN<Text::String> s;
	if (i == INVALID_INDEX || !this->itemTexts.GetItem(i).SetTo(s))
		return 0;
	return Text::StrConcatC(sbuff, s->v, s->leng);
}

UTF8Char *UI::GUIComboBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	NN<Text::String> txt;
	if (!this->itemTexts.GetItem(index).SetTo(txt))
		return 0;
	return Text::StrConcatC(buff, txt->v, txt->leng);
}

Text::CStringNN UI::GUIComboBox::GetObjectClass() const
{
	return CSTR("ComboBox");
}

void UI::GUIComboBox::HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUIComboBox::SetAutoComplete(Bool autoComplete)
{
	if (!this->autoComplete && autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(sb);
		this->lastTextLeng = sb.GetLength();
	}
	this->autoComplete = autoComplete;
}
