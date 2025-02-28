#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRLogFileForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

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
		UnsafeArray<UTF8Char> sptr;
		Data::Timestamp ts = 0;
		Text::StringBuilderUTF8 sb;
		while (i < j)
		{
			sb.ClearStr();
			this->logFile->GetLogMessage(this->logLevel, i, &ts, sb, Text::LineBreakType::CRLF);
			sptr = ts.ToLocalTime().ToString(sbuff);
			k = this->lvLogs->AddItem(CSTRP(sbuff, sptr), 0);
			this->lvLogs->SetSubItem(k, 1, sb.ToCString());

			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLogFileForm::OnLogsDblClk(AnyType userObj, UOSInt itemIndex)
{
	NN<SSWR::AVIRead::AVIRLogFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogFileForm>();
	Text::StringBuilderUTF8 sb;
	me->logFile->GetLogDescription(me->logLevel, itemIndex, sb);
	me->ui->ShowMsgOK(sb.ToCString(), CSTR("Log Detail"), me);
}

SSWR::AVIRead::AVIRLogFileForm::AVIRLogFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::LogFile> logFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->logFile = logFile;
	this->logLevel = IO::LogHandler::LogLevel::Raw;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	sptr = logFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("LogFile Viewer - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->SetFont(0, 0, 8.25, false);

	this->lvLogs = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvLogs->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogs->SetShowGrid(true);
	this->lvLogs->SetFullRowSelect(true);
	this->lvLogs->AddColumn(CSTR("Time"), 200);
	this->lvLogs->AddColumn(CSTR("Message"), 812);
	this->lvLogs->HandleDblClk(OnLogsDblClk, this);

	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Save As..."), MNU_LOG_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	this->UpdateLogMessages();
}

SSWR::AVIRead::AVIRLogFileForm::~AVIRLogFileForm()
{
	this->logFile.Delete();
}

void SSWR::AVIRead::AVIRLogFileForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	switch (cmdId)
	{
	case MNU_LOG_SAVE:
		{
			NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LogFileSave", true);
			dlg->AddFilter(CSTR("*.txt"), CSTR("Log Text file"));
			if (dlg->ShowDialog(this->hwnd))
			{
				Data::Timestamp ts = 0;
				Text::StringBuilderUTF8 sb;
				UOSInt i;
				UOSInt j;
				IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Text::UTF8Writer writer(fs);
				i = 0;
				j = this->logFile->GetCount(this->logLevel);
				while (i < j)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("\t"));
					this->logFile->GetLogMessage(this->logLevel, i, &ts, sb, Text::LineBreakType::CRLF);
					sptr = ts.ToLocalTime().ToString(sbuff);
					writer.Write(CSTRP(sbuff, sptr));
					writer.WriteLine(sb.ToCString());
					i++;
				}
			}
			dlg.Delete();
		}
		break;
	}
}

void SSWR::AVIRead::AVIRLogFileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
