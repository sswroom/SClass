#include "Stdafx.h"
#include "Data/ArrayListStringNN.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentBatchForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Clipboard.h"

void SSWR::AVIRead::AVIRUserAgentBatchForm::UserAgent2Output(Text::CString userAgent, NotNullPtr<Text::StringBuilderUTF8> outSb)
{
	Text::StringBuilderUTF8 sb;
	Net::UserAgentDB::UAEntry ent;
	UOSInt j;
	Net::UserAgentDB::ParseUserAgent(&ent, userAgent);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("\t{Net::BrowserInfo::"));
	sb.Append(Net::BrowserInfo::GetDefName(ent.browser));
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 35) sb.AppendChar(' ', 35 - j);

	if (ent.browserVer)
	{
		sb.AppendC(UTF8STRC("UTF8STRC(\""));
		sb.AppendC(ent.browserVer, ent.browserVerLen);
		sb.AppendC(UTF8STRC("\")"));
	}
	else
	{
		sb.AppendC(UTF8STRC("0, 0"));
	}
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 62) sb.AppendChar(' ', 62 - j);

	sb.AppendC(UTF8STRC("Manage::OSInfo::"));
	sb.Append(Manage::OSInfo::GetDefName(ent.os));
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 95) sb.AppendChar(' ', 95 - j);

	if (ent.osVer)
	{
		sb.AppendC(UTF8STRC("UTF8STRC(\""));
		sb.AppendC(ent.osVer, ent.osVerLen);
		sb.AppendC(UTF8STRC("\")"));
	}
	else
	{
		sb.AppendC(UTF8STRC("0, 0"));
	}
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 118) sb.AppendChar(' ', 118 - j);

	if (ent.devName)
	{
		sb.AppendC(UTF8STRC("UTF8STRC(\""));
		sb.AppendC(ent.devName, ent.devNameLen);
		sb.AppendC(UTF8STRC("\")"));
	}
	else
	{
		sb.AppendC(UTF8STRC("0, 0"));
	}
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 140) sb.AppendChar(' ', 140 - j);
	NotNullPtr<Text::String> s = Text::JSText::ToNewJSTextDQuote((const UTF8Char*)ent.userAgent);
	sb.AppendC(UTF8STRC("UTF8STRC("));
	sb.Append(s);
	s->Release();
	sb.AppendC(UTF8STRC(")},\r\n"));
	outSb->AppendC(sb.ToString(), sb.GetLength());

	SDEL_TEXT(ent.browserVer);
	SDEL_TEXT(ent.osVer);
	SDEL_TEXT(ent.devName);
}

void __stdcall SSWR::AVIRead::AVIRUserAgentBatchForm::OnParseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentBatchForm *me = (SSWR::AVIRead::AVIRUserAgentBatchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb3;
	Text::PString sarr[2];
	UOSInt i;
	me->txtSource->GetText(sb);
	if (sb.GetLength() <= 0)
	{
		me->txtOutput->SetText(CSTR(""));
		return;
	}
	sarr[1] = sb;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (sarr[0].v[0] != 0)
		{
			UserAgent2Output(sarr[0].ToCString(), sb3);
		}

		if (i != 2)
		{
			break;
		}
	}
	me->txtOutput->SetText(sb3.ToCString());
	me->txtOutput->SelectAll();
	me->txtOutput->Focus();
}

void __stdcall SSWR::AVIRead::AVIRUserAgentBatchForm::OnUpdateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentBatchForm *me = (SSWR::AVIRead::AVIRUserAgentBatchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSource->GetText(sb);
	me->UpdateByText(sb);
}

void __stdcall SSWR::AVIRead::AVIRUserAgentBatchForm::OnUpdateCBClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentBatchForm *me = (SSWR::AVIRead::AVIRUserAgentBatchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::Clipboard::GetString(me->GetHandle(), sb);
	if (sb.GetLength() > 0)
	{
		me->UpdateByText(sb);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("No text in clipboard"), CSTR("User Agent Update"), me);
	}
}

void SSWR::AVIRead::AVIRUserAgentBatchForm::UpdateByText(Text::PString txt)
{
	Data::ArrayListStringNN uaList;
	UOSInt i;
	UOSInt j;
	OSInt k;
	Net::UserAgentDB::UAEntry *entList = Net::UserAgentDB::GetUAEntryList(&j);
	Text::PString sarr[2];
	Bool found;
	i = 0;
	while (i < j)
	{
		UOSInt uaLen = Text::StrCharCnt(entList[i].userAgent);
		k = uaList.SortedIndexOfC(Text::CStringNN((const UTF8Char*)entList[i].userAgent, uaLen));
		if (k < 0)
		{
			uaList.Insert((UOSInt)~k, Text::String::New((const UTF8Char*)entList[i].userAgent, uaLen));
		}
		i++;
	}
	sarr[1] = txt;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (sarr[0].v[0] != 0)
		{
			k = uaList.SortedIndexOfC(sarr[0].ToCString());
			if (k < 0)
			{
				uaList.Insert((UOSInt)~k, Text::String::New(sarr[0].v, sarr[0].leng));
				found = true;
			}
		}

		if (i != 2)
		{
			break;
		}
	}

	if (found)
	{
		Text::StringBuilderUTF8 sb;
		sb.ClearStr();
		Data::ArrayIterator<NotNullPtr<Text::String>> it = uaList.Iterator();
		while (it.HasNext())
		{
			UserAgent2Output(it.Next()->ToCString(), sb);
		}
		this->txtOutput->SetText(sb.ToCString());
		this->txtOutput->SelectAll();
		this->txtOutput->Focus();
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Nothing to update"), CSTR("User Agent Update"), this);
	}
	
	uaList.FreeAll();
}

SSWR::AVIRead::AVIRUserAgentBatchForm::AVIRUserAgentBatchForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("User Agent Batch"));

	this->core = core;

	NEW_CLASSNN(this->pnlSource, UI::GUIPanel(ui, *this));
	this->pnlSource->SetRect(0, 0, 100, 256, false);
	this->pnlSource->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASSNN(this->pnlControl, UI::GUIPanel(ui, this->pnlSource));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->txtSource, UI::GUITextBox(ui, this->pnlSource, CSTR(""), true));
	this->txtSource->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnParse, UI::GUIButton(ui, this->pnlControl, CSTR("Parse")));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	NEW_CLASS(this->btnUpdate, UI::GUIButton(ui, this->pnlControl, CSTR("Update")));
	this->btnUpdate->SetRect(84, 4, 75, 23, false);
	this->btnUpdate->HandleButtonClick(OnUpdateClicked, this);
	NEW_CLASS(this->btnUpdateCB, UI::GUIButton(ui, this->pnlControl, CSTR("Update from Clipboard")));
	this->btnUpdateCB->SetRect(164, 4, 150, 23, false);
	this->btnUpdateCB->HandleButtonClick(OnUpdateCBClicked, this);
	NEW_CLASS(this->vspControl, UI::GUIVSplitter(ui, *this, 3, false));
	NEW_CLASS(this->txtOutput, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtOutput->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtOutput->SetReadOnly(true);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::AVIRead::AVIRUserAgentBatchForm::~AVIRUserAgentBatchForm()
{
}

void SSWR::AVIRead::AVIRUserAgentBatchForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
