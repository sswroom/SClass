#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Manage/Process.h"
#include "SSWR/ProcMonForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

void SSWR::ProcMonForm::AddProg(const UTF8Char *progName, const UTF8Char *progPath)
{
	SSWR::ProcMonForm::ProgInfo *prog;
	prog = MemAlloc(ProgInfo, 1);
	prog->progName = Text::StrCopyNew(progName);
	prog->procId = 0;
	if (progPath)
	{
		prog->progPath = Text::String::NewNotNull(progPath);
	}
	else
	{
		prog->progPath = 0;
	}
	this->progList->Add(prog);
	this->lbProg->AddItem(prog->progName, prog);

	if (progPath)
	{
		this->SearchProcId(prog);
	}
}

Bool SSWR::ProcMonForm::SearchProcId(SSWR::ProcMonForm::ProgInfo *prog)
{
	if (prog->progPath == 0)
		return false;

	UTF8Char sbuff[512];
	UOSInt i;
	Bool ret = false;
	Manage::Process::ProcessInfo info;
	i = prog->progPath->LastIndexOf(IO::Path::PATH_SEPERATOR);
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(&prog->progPath->v[i + 1]);
	if (sess)
	{
		Text::StringBuilderUTF8 sb;
		while (Manage::Process::FindProcessNext(sbuff, sess, &info))
		{
			Manage::Process proc(info.processId, false);
			sb.ClearStr();
			if (proc.GetFilename(&sb))
			{
				if (sb.Equals(prog->progPath))
				{
					Text::StringBuilderUTF8 sb;
					prog->procId = info.processId;
					ret = true;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(": Updated procId as "));
					sb.AppendUOSInt(prog->procId);
					this->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
					break;
				}
			}
		}
		Manage::Process::FindProcessClose(sess);
	}
	return ret;
}

void SSWR::ProcMonForm::SetByProcId(ProgInfo *prog, UOSInt procId)
{
	Manage::Process proc(procId, false);
	if (proc.IsRunning())
	{
		Text::StringBuilderUTF8 sb;
		if (proc.GetFilename(&sb))
		{
			SDEL_STRING(prog->progPath);
			prog->progPath = Text::String::New(sb.ToString(), sb.GetLength());
			prog->procId = procId;
			this->txtProgPath->SetText(sb.ToString());
			this->SaveProgList();
		}
	}
}

void SSWR::ProcMonForm::LoadProgList()
{
	UTF8Char sbuff[512];
	UTF8Char *sarr[2];
	IO::FileStream *fs;
	IO::StreamReader *reader;
	Text::StringBuilderUTF8 sb;

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"prg");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, IO::StreamReader(fs, 65001));
		
		while (true)
		{
			sb.ClearStr();
			if (!reader->ReadLine(&sb, 4096))
				break;
			if (Text::StrSplit(sarr, 2, sb.ToString(), ',') == 2)
			{
				if (sarr[1][0])
				{
					this->AddProg(sarr[0], sarr[1]);
				}
				else
				{
					this->AddProg(sarr[0], sarr[1]);
				}
			}
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}

void SSWR::ProcMonForm::SaveProgList()
{
	UTF8Char sbuff[512];
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	ProgInfo *prog;

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"prg");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyAll, IO::FileStream::BufferType::NoWriteBuffer));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	i = 0;
	j = this->progList->GetCount();
	while (i < j)
	{
		prog = this->progList->GetItem(i);
		sb.ClearStr();
		sb.Append(prog->progName);
		sb.AppendC(UTF8STRC(","));
		if (prog->progPath)
		{
			sb.Append(prog->progPath);
		}
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(fs);
}

void __stdcall SSWR::ProcMonForm::OnProgSelChange(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	UTF8Char sbuff[32];
	ProgInfo *prog = (ProgInfo*)me->lbProg->GetSelectedItem();
	if (prog && prog->progPath)
	{
		me->txtProgPath->SetText(prog->progPath->v);
	}
	else
	{
		me->txtProgPath->SetText((const UTF8Char*)"");
	}
	if (prog)
	{
		Text::StrUOSInt(sbuff, prog->procId);
		me->txtProcId->SetText(sbuff);
	}
	else
	{
		me->txtProcId->SetText((const UTF8Char*)"");
	}
}

void __stdcall SSWR::ProcMonForm::OnProcIdClicked(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	ProgInfo *prog = (ProgInfo*)me->lbProg->GetSelectedItem();
	if (prog)
	{
		Text::StringBuilderUTF8 sb;
		UInt32 procId;
		me->txtProcId->GetText(&sb);
		if (sb.ToUInt32(&procId))
		{
			me->SetByProcId(prog, procId);
		}
	}
}

void __stdcall SSWR::ProcMonForm::OnProgAddClicked(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 procId;
	me->txtProgAddId->GetText(&sb);
	if (sb.ToUInt32(&procId))
	{
		sb.ClearStr();
		me->txtProgAddName->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			Manage::Process proc(procId, false);
			if (proc.IsRunning())
			{
				Text::StringBuilderUTF8 sb2;
				if (proc.GetFilename(&sb2))
				{
					me->AddProg(sb.ToString(), sb2.ToString());
					me->SaveProgList();
					me->txtProgAddId->SetText((const UTF8Char*)"");
				}
			}
		}
	}
}

void __stdcall SSWR::ProcMonForm::OnLogSelChg(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	Text::String *s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->v);
	s->Release();
}

void __stdcall SSWR::ProcMonForm::OnTimerTick(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	UOSInt i;
	ProgInfo *prog;
	i = me->progList->GetCount();
	while (i-- > 0)
	{
		prog = me->progList->GetItem(i);
		if (prog->progPath != 0)
		{
			if (prog->procId != 0)
			{
				Manage::Process proc(prog->procId, false);
				if (!proc.IsRunning())
				{
					prog->procId = 0;
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(" stopped"));
					me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
				}
			}
			if (prog->procId == 0)
			{
				if (!me->SearchProcId(prog))
				{
					Manage::Process proc(prog->progPath->v);
					if (proc.IsRunning())
					{
						prog->procId = proc.GetProcId();
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Prog "));
						sb.Append(prog->progName);
						sb.AppendC(UTF8STRC(" restarted, procId = "));
						sb.AppendUOSInt(prog->procId);
						me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
						
						if (me->notifyCmd)
						{
							sb.ClearStr();
							sb.Append(me->notifyCmd);
							sb.AppendC(UTF8STRC(" \"Prog "));
							sb.Append(prog->progName);
							sb.AppendC(UTF8STRC(" restarted\""));
							Manage::Process proc2(sb.ToString());
						}
					}
				}
			}
		}
	}
}

SSWR::ProcMonForm::ProcMonForm(UI::GUIClientControl *parent, UI::GUICore *ui) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Process Monitor");
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->tpProg = this->tcMain->AddTabPage((const UTF8Char*)"Prog");
	NEW_CLASS(this->lbProg, UI::GUIListBox(ui, this->tpProg, false));
	this->lbProg->SetRect(0, 0, 150, 100, false);
	this->lbProg->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProg->HandleSelectionChange(OnProgSelChange, this);
	NEW_CLASS(this->pnlProg, UI::GUIPanel(ui, this->tpProg));
	this->pnlProg->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->grpProgAdd, UI::GUIGroupBox(ui, this->pnlProg, (const UTF8Char*)"Add Prog"));
	this->grpProgAdd->SetRect(0, 0, 100, 64, false);
	this->grpProgAdd->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblProgAddName, UI::GUILabel(ui, this->grpProgAdd, (const UTF8Char*)"Prog Name"));
	this->lblProgAddName->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtProgAddName, UI::GUITextBox(ui, this->grpProgAdd, (const UTF8Char*)""));
	this->txtProgAddName->SetRect(100, 0, 200, 23, false);
	NEW_CLASS(this->lblProgAddId, UI::GUILabel(ui, this->grpProgAdd, (const UTF8Char*)"Process Id"));
	this->lblProgAddId->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtProgAddId, UI::GUITextBox(ui, this->grpProgAdd, (const UTF8Char*)""));
	this->txtProgAddId->SetRect(100, 24, 100, 23, false);
	NEW_CLASS(this->btnProgAdd, UI::GUIButton(ui, this->grpProgAdd, (const UTF8Char*)"&Add"));
	this->btnProgAdd->SetRect(200, 24, 75, 23, false);
	this->btnProgAdd->HandleButtonClick(OnProgAddClicked, this);
	NEW_CLASS(this->lblProcId, UI::GUILabel(ui, this->pnlProg, (const UTF8Char*)"Process Id"));
	this->lblProcId->SetRect(4, 68, 100, 23, false);
	NEW_CLASS(this->txtProcId, UI::GUITextBox(ui, this->pnlProg, (const UTF8Char*)""));
	this->txtProcId->SetRect(104, 68, 100, 23, false);
	NEW_CLASS(this->btnProcId, UI::GUIButton(ui, this->pnlProg, (const UTF8Char*)"Set"));
	this->btnProcId->SetRect(204, 68, 75, 23, false);
	this->btnProcId->HandleButtonClick(OnProcIdClicked, this);
	NEW_CLASS(this->lblProgPath, UI::GUILabel(ui, this->pnlProg, (const UTF8Char*)"Path"));
	this->lblProgPath->SetRect(4, 92, 100, 23, false);
	NEW_CLASS(this->txtProgPath, UI::GUITextBox(ui, this->pnlProg, (const UTF8Char*)""));
	this->txtProgPath->SetRect(104, 92, 600, 23, false);
	this->txtProgPath->SetReadOnly(true);

	this->notifyCmd = 0;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Log"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendC(UTF8STRC("ProgLog"));
	NEW_CLASS(this->log, IO::LogTool());
	this->log->AddFileLog(sb.ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 512, false));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
	NEW_CLASS(this->progList, Data::ArrayList<ProgInfo*>());
	this->LoadProgList();

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		Text::String *s = cfg->GetValue(UTF8STRC("NotifyCmd"));
		if (s)
		{
			this->notifyCmd = s->Clone();
		}
		DEL_CLASS(cfg);
	}
	this->AddTimer(30000, OnTimerTick, this);
}

SSWR::ProcMonForm::~ProcMonForm()
{
	ProgInfo *prog;
	UOSInt i = this->progList->GetCount();
	while (i-- > 0)
	{
		prog = this->progList->GetItem(i);
		SDEL_STRING(prog->progPath);
		SDEL_TEXT(prog->progName);
		MemFree(prog);
	}
	DEL_CLASS(this->progList);
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
	SDEL_STRING(this->notifyCmd);
}
