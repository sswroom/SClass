#include "Stdafx.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "SSWR/AVIRead/AVIRCppEnumForm.h"
#include "Text/CPPText.h"
#include "Text/MyString.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRCppEnumForm::OnConvClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCppEnumForm *me = (SSWR::AVIRead::AVIRCppEnumForm*)userObj;
	me->ConvEnum();
}

void __stdcall SSWR::AVIRead::AVIRCppEnumForm::OnConv2Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRCppEnumForm *me = (SSWR::AVIRead::AVIRCppEnumForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Win32::Clipboard clipboard(me->GetHandle());
	if (clipboard.GetString(me->GetHandle(), &sb))
	{
		me->txtSource->SetText(sb.ToCString());
	}
	else
	{
		return;
	}
	me->ConvEnum();
	sb.ClearStr();
	me->txtDest->GetText(&sb);
	clipboard.SetString(me->GetHandle(), sb.ToString());
}

void SSWR::AVIRead::AVIRCppEnumForm::ConvEnum()
{
	Text::StringBuilderUTF8 srcSb;
	Text::StringBuilderUTF8 destSb;
	Text::StringBuilderUTF8 sbPrefix;
	Data::ArrayList<const UTF8Char*> enumList;

	this->txtPrefix->GetText(&sbPrefix);
	this->txtSource->GetText(&srcSb);
	if (Text::CPPText::ParseEnum(&enumList, srcSb.ToString(), &sbPrefix))
	{
		UOSInt i = 0;
		UOSInt j = enumList.GetCount();
		while (i < j)
		{
			destSb.AppendC(UTF8STRC("case "));
			destSb.AppendC(sbPrefix.ToString(), sbPrefix.GetLength());
			destSb.AppendSlow(enumList.GetItem(i));
			destSb.AppendC(UTF8STRC(":\r\n"));
			i++;
		}
		this->txtDest->SetText(destSb.ToCString());
	}
	else
	{
		this->txtDest->SetText(CSTR(""));
	}
	LIST_FREE_FUNC(&enumList, Text::StrDelNew);
}

SSWR::AVIRead::AVIRCppEnumForm::AVIRCppEnumForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("C++ Enum to Switch case"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnConv, UI::GUIButton(ui, this->pnlCtrl, CSTR("Convert")));
	this->btnConv->SetRect(4, 4, 75, 23, false);
	this->btnConv->HandleButtonClick(OnConvClicked, this);
	NEW_CLASS(this->btnConv2, UI::GUIButton(ui, this->pnlCtrl, CSTR("Paste-Conv-Copy")));
	this->btnConv2->SetRect(84, 4, 150, 23, false);
	this->btnConv2->HandleButtonClick(OnConv2Clicked, this);
	NEW_CLASS(this->lblPrefix, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Prefix"));
	this->lblPrefix->SetRect(234, 4, 100, 23, false);
	NEW_CLASS(this->txtPrefix, UI::GUITextBox(ui, this->pnlCtrl, CSTR("")));
	this->txtPrefix->SetRect(334, 4, 100, 23, false);
	NEW_CLASS(this->txtSource, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtSource->SetRect(0, 0, 512, 100, false);
	this->txtSource->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtDest, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtDest->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDest->SetReadOnly(true);
}

SSWR::AVIRead::AVIRCppEnumForm::~AVIRCppEnumForm()
{
}

void SSWR::AVIRead::AVIRCppEnumForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
