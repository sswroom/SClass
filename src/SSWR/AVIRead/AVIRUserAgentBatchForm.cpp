#include "Stdafx.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentBatchForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void SSWR::AVIRead::AVIRUserAgentBatchForm::UserAgent2Output(const UTF8Char *userAgent, Text::StringBuilderUTF8 *outSb)
{
	Text::StringBuilderUTF8 sb;
	Net::UserAgentDB::UAEntry ent;
	UOSInt j;
	Net::UserAgentDB::ParseUserAgent(&ent, userAgent);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("\t{Net::BrowserInfo::"));
	sb.Append(Net::BrowserInfo::GetDefName(ent.browser));
	sb.AppendChar(',', 1);
	j = sb.GetLength();
	if (j < 35) sb.AppendChar(' ', 35 - j);

	if (ent.browserVer)
	{
		sb.AppendChar('"', 1);
		sb.AppendSlow((const UTF8Char*)ent.browserVer);
		sb.AppendChar('"', 1);
	}
	else
	{
		sb.AppendChar('0', 1);
	}
	sb.AppendChar(',', 1);
	j = sb.GetLength();
	if (j < 52) sb.AppendChar(' ', 52 - j);

	sb.AppendC(UTF8STRC("Manage::OSInfo::"));
	sb.Append(Manage::OSInfo::GetDefName(ent.os));
	sb.AppendChar(',', 1);
	j = sb.GetLength();
	if (j < 85) sb.AppendChar(' ', 85 - j);

	if (ent.osVer)
	{
		sb.AppendChar('"', 1);
		sb.AppendSlow((const UTF8Char*)ent.osVer);
		sb.AppendChar('"', 1);
	}
	else
	{
		sb.AppendChar('0', 1);
	}
	sb.AppendChar(',', 1);
	j = sb.GetLength();
	if (j < 98) sb.AppendChar(' ', 98 - j);

	if (ent.devName)
	{
		sb.AppendChar('"', 1);
		sb.AppendSlow((const UTF8Char*)ent.devName);
		sb.AppendChar('"', 1);
	}
	else
	{
		sb.AppendChar('0', 1);
	}
	sb.AppendChar(',', 1);
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
	UTF8Char *sarr[2];
	UOSInt i;
	me->txtSource->GetText(&sb);
	if (sb.GetLength() <= 0)
	{
		me->txtOutput->SetText((const UTF8Char*)"");
		return;
	}
	sarr[1] = sb.ToString();
	while (true)
	{
		i = Text::StrSplitLine(sarr, 2, sarr[1]);
		if (sarr[0][0] != 0)
		{
			UserAgent2Output(sarr[0], &sb3);
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
	me->UpdateByText(sb.ToString());
}

void __stdcall SSWR::AVIRead::AVIRUserAgentBatchForm::OnUpdateCBClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentBatchForm *me = (SSWR::AVIRead::AVIRUserAgentBatchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Win32::Clipboard::GetString(me->GetHandle(), &sb);
	if (sb.GetLength() > 0)
	{
		me->UpdateByText(sb.ToString());
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"No text in clipboard", (const UTF8Char*)"User Agent Update", me);
	}
}

void SSWR::AVIRead::AVIRUserAgentBatchForm::UpdateByText(UTF8Char *txt)
{
	Data::ArrayListStrUTF8 uaList;
	UOSInt i;
	UOSInt j;
	OSInt k;
	Net::UserAgentDB::UAEntry *entList = Net::UserAgentDB::GetUAEntryList(&j);
	UTF8Char *sarr[2];
	Bool found;
	i = 0;
	while (i < j)
	{
		k = uaList.SortedIndexOf((const UTF8Char*)entList[i].userAgent);
		if (k < 0)
		{
			uaList.Insert((UOSInt)~k, Text::StrCopyNew((const UTF8Char*)entList[i].userAgent));
		}
		i++;
	}
	sarr[1] = txt;
	while (true)
	{
		i = Text::StrSplitLine(sarr, 2, sarr[1]);
		if (sarr[0][0] != 0)
		{
			k = uaList.SortedIndexOf(sarr[0]);
			if (k < 0)
			{
				uaList.Insert((UOSInt)~k, Text::StrCopyNew(sarr[0]));
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
			UserAgent2Output(uaList.GetItem(i), &sb);
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
	
	i = uaList.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(uaList.GetItem(i));
	}
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
	NEW_CLASS(this->txtSource, UI::GUITextBox(ui, this->pnlSource, (const UTF8Char*)"", true));
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
	NEW_CLASS(this->txtOutput, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
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
