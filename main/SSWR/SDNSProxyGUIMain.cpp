#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRDNSProxyForm.h"
#include "UI/GUICore.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::GUICore *ui;
	SSWR::AVIRead::AVIRDNSProxyForm *frm;
	SSWR::AVIRead::AVIRCore *core;
	Manage::ExceptionRecorder *exHdlr;
	IO::ConfigFile *cfg;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile((const UTF8Char*)"Memory.log");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"SDNSProxy.log", Manage::ExceptionRecorder::EA_RESTART));
	ui = Core::IProgControl::CreateGUICore(progCtrl);
	NEW_CLASS(core, SSWR::AVIRead::AVIRCoreWin(ui));
	NEW_CLASS(frm, SSWR::AVIRead::AVIRDNSProxyForm(0, ui, core));
	frm->SetExitOnClose(true);
	frm->Show();
	cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		Text::String *s;
		UOSInt i;
		UInt32 ip;
		Int32 v;
		UTF8Char *sarr[2];
		s = cfg->GetValue((const UTF8Char*)"DNS");
		if (s)
		{
			Data::ArrayList<UInt32> dnsList;
			Text::StringBuilderUTF8 sb;
			sb.Append(s);
			sarr[1] = sb.ToString();
			while (true)
			{
				i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
				ip = Net::SocketUtil::GetIPAddr(sarr[0]);
				if (ip)
				{
					dnsList.Add(ip);
				}
				if (i <= 1)
					break;
			}
			frm->SetDNSList(&dnsList);
		}

		s = cfg->GetValue((const UTF8Char*)"DisableV6");
		if (s && s->ToInt32(&v))
		{
			frm->SetDisableV6(v != 0);
		}

		s = cfg->GetValue((const UTF8Char*)"Blacklist");
		if (s && s->leng != 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(s);
			sarr[1] = sb.ToString();
			while (true)
			{
				i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
				frm->AddBlacklist(sarr[0]);
				if (i <= 1)
					break;
			}
		}

		DEL_CLASS(cfg);
	}
	ui->Run();

	DEL_CLASS(core);
	DEL_CLASS(ui);
	DEL_CLASS(exHdlr);
	return 0;
}
