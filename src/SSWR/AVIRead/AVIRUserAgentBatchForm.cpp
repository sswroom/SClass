#include "Stdafx.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentBatchForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void SSWR::AVIRead::AVIRUserAgentBatchForm::UserAgent2Output(Text::CString userAgent, Text::StringBuilderUTF8 *outSb)
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
		sb.AppendUTF8Char('"');
		sb.AppendSlow((const UTF8Char*)ent.browserVer);
		sb.AppendUTF8Char('"');
	}
	else
	{
		sb.AppendUTF8Char('0');
	}
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 52) sb.AppendChar(' ', 52 - j);

	sb.AppendC(UTF8STRC("Manage::OSInfo::"));
	sb.Append(Manage::OSInfo::GetDefName(ent.os));
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 85) sb.AppendChar(' ', 85 - j);

	if (ent.osVer)
	{
		sb.AppendUTF8Char('"');
		sb.AppendSlow((const UTF8Char*)ent.osVer);
		sb.AppendUTF8Char('"');
	}
	else
	{
		sb.AppendUTF8Char('0');
	}
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 98) sb.AppendChar(' ', 98 - j);

	if (ent.devName)
	{
		sb.AppendUTF8Char('"');
		sb.AppendSlow((const UTF8Char*)ent.devName);
		sb.AppendUTF8Char('"');
	}
	else
	{
		sb.AppendUTF8Char('0');
	}
	sb.AppendUTF8Char(',');
	j = sb.GetLength();
	if (j < 110) sb.AppendChar(' ', 110 - j);
	Text::String *s = Text::JSText::ToNewJSTextDQuote((const UTF8Char*)ent.userAgent);
	sb.Append(s);
	s->Release();
	sb.AppendC(UTF8STRC("},\r\n"));
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
	me->txtSource->GetText(&sb);
	if (sb.GetLength() <= 0)
	{
		me->txtOutput->SetText((const UTF8Char*)"");
		return;
	}
	sarr[1] = sb;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (sarr[0].v[0] != 0)
		{
			UserAgent2Output(sarr[0].ToCString(), &sb3);
		}

		if (i != 2)
		{
			break;
		}
	}
	me->txtOutput->SetText(sb3.ToString());
	me->txtOutput->SelectAll();
	me->txtOutput->Focus();
}

void __stdcall SSWR::AVIRead::AVIRUserAgentBatchForm::OnUpdateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentBatchForm *me = (SSWR::AVIRead::AVIRUserAgentBatchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSource->GetText(&sb);
	me->UpdateByText(sb.ToString(), sb.GetLength());
}

void __stdcall SSWR::AVIRead::AVIRUserAgentBatchForm::OnUpdateCBClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentBatchForm *me = (SSWR::AVIRead::AVIRUserAgentBatchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Win32::Clipboard::GetString(me->GetHandle(), &sb);
	if (sb.GetLength() > 0)
	{
		me->UpdateByText(sb.ToString(), sb.GetLength());
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"No text in clipboard", (const UTF8Char*)"User Agent Update", me);
	}
}

void SSWR::AVIRead::AVIRUserAgentBatchForm::UpdateByText(UTF8Char *txt, UOSInt txtLen)
{
	Data::ArrayListString uaList;
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
		k = uaList.SortedIndexOfPtr((const UTF8Char*)entList[i].userAgent, uaLen);
		if (k < 0)
		{
			uaList.Insert((UOSInt)~k, Text::String::New((const UTF8Char*)entList[i].userAgent, uaLen));
		}
		i++;
	}
	sarr[1].v = txt;
	sarr[1].leng = txtLen;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (sarr[0].v[0] != 0)
		{
			k = uaList.SortedIndexOfPtr(sarr[0].v, sarr[0].leng);
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
		i = 0;
		j = uaList.GetCount();
		while (i < j)
		{
			UserAgent2Output(uaList.GetItem(i)->ToCString(), &sb);
			i++;
		}
		this->txtOutput->SetText(sb.ToString());
		this->txtOutput->SelectAll();
		this->txtOutput->Focus();
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Nothing to update", (const UTF8Char*)"User Agent Update", this);
	}
	
	LIST_FREE_STRING(&uaList);
}

SSWR::AVIRead::AVIRUserAgentBatchForm::AVIRUserAgentBatchForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"User Agent Batch");

	this->core = core;

	NEW_CLASS(this->pnlSource, UI::GUIPanel(ui, this));
	this->pnlSource->SetRect(0, 0, 100, 256, false);
	this->pnlSource->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this->pnlSource));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->txtSource, UI::GUITextBox(ui, this->pnlSource, CSTR(""), true));
	this->txtSource->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnParse, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Parse"));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	NEW_CLASS(this->btnUpdate, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Update"));
	this->btnUpdate->SetRect(84, 4, 75, 23, false);
	this->btnUpdate->HandleButtonClick(OnUpdateClicked, this);
	NEW_CLASS(this->btnUpdateCB, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Update from Clipboard"));
	this->btnUpdateCB->SetRect(164, 4, 150, 23, false);
	this->btnUpdateCB->HandleButtonClick(OnUpdateCBClicked, this);
	NEW_CLASS(this->vspControl, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtOutput, UI::GUITextBox(ui, this, CSTR(""), true));
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
