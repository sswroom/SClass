#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRLogFileForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

typedef enum
{
	MNU_LOG_SAVE = 101
} MenuItems;

void SSWR::AVIRead::AVIRLogFileForm::UpdateLogMessages()
{
	this->lvLogs->ClearItems();
	UOSInt i;
	UOSInt j = this->logFile->GetCount(this->logLevel);
	UOSInt k;
	i = 0;
	if (j > 0)
	{
		UTF8Char sbuff[64];
		Text::StringBuilderUTF8 *sb;
		Data::DateTime *dt;

		NEW_CLASS(dt, Data::DateTime());
		NEW_CLASS(sb, Text::StringBuilderUTF8());
		while (i < j)
		{
			sb->ClearStr();
			this->logFile->GetLogMessage(this->logLevel, i, dt, sb, Text::LBT_CRLF);
			dt->ToLocalTime();
			dt->ToString(sbuff);
			k = this->lvLogs->AddItem(sbuff, 0);
			this->lvLogs->SetSubItem(k, 1, sb->ToString());

			i++;
		}
		DEL_CLASS(sb);
		DEL_CLASS(dt);
	}
}

void __stdcall SSWR::AVIRead::AVIRLogFileForm::OnLogsDblClk(void *userObj, UOSInt itemIndex)
{
	SSWR::AVIRead::AVIRLogFileForm *me = (SSWR::AVIRead::AVIRLogFileForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->logFile->GetLogDescription(me->logLevel, itemIndex, &sb);
	UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char *)"Log Detail", me);
}

SSWR::AVIRead::AVIRLogFileForm::AVIRLogFileForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::LogFile *logFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	this->logFile = logFile;
	this->logLevel = IO::ILogHandler::LOG_LEVEL_RAW;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"LogFile Viewer - "), logFile->GetSourceNameObj());
	this->SetText(sbuff);
	this->SetFont(0, 8.25, false);

	NEW_CLASS(this->lvLogs, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvLogs->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogs->SetShowGrid(true);
	this->lvLogs->SetFullRowSelect(true);
	this->lvLogs->AddColumn((const UTF8Char*)"Time", 200);
	this->lvLogs->AddColumn((const UTF8Char*)"Message", 812);
	this->lvLogs->HandleDblClk(OnLogsDblClk, this);

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&File");
	mnu->AddItem((const UTF8Char*)"&Save As...", MNU_LOG_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	this->UpdateLogMessages();
}

SSWR::AVIRead::AVIRLogFileForm::~AVIRLogFileForm()
{
	DEL_CLASS(this->logFile);
}

void SSWR::AVIRead::AVIRLogFileForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[64];
	switch (cmdId)
	{
	case MNU_LOG_SAVE:
		{
			UI::FileDialog *dlg;
			NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"LogFileSave", true));
			dlg->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Log Text file");
			if (dlg->ShowDialog(this->hwnd))
			{
				Data::DateTime dt;
				IO::FileStream *fs;
				Text::UTF8Writer *writer;
				Text::StringBuilderUTF8 *sb;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(sb, Text::StringBuilderUTF8());
				NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				NEW_CLASS(writer, Text::UTF8Writer(fs));
				i = 0;
				j = this->logFile->GetCount(this->logLevel);
				while (i < j)
				{
					sb->ClearStr();
					sb->Append((const UTF8Char*)"\t");
					this->logFile->GetLogMessage(this->logLevel, i, &dt, sb, Text::LBT_CRLF);
					dt.ToLocalTime();
					dt.ToString(sbuff);
					writer->Write(sbuff);
					writer->WriteLine(sb->ToString());
					i++;
				}
				DEL_CLASS(writer);
				DEL_CLASS(fs);
				DEL_CLASS(sb);
			}
			DEL_CLASS(dlg);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRLogFileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
