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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	SSWR::AVIRead::AVIRDNSProxyForm *frm;
	NN<SSWR::AVIRead::AVIRCore> core;
	Manage::ExceptionRecorder *exHdlr;
	NN<IO::ConfigFile> cfg;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile(UTF8STRCPTR("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SDNSProxy.log"), Manage::ExceptionRecorder::EA_RESTART));
	if (Core::ProgControl::CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASS(frm, SSWR::AVIRead::AVIRDNSProxyForm(nullptr, ui, core));
		frm->SetExitOnClose(true);
		frm->Show();
		if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
		{
			NN<Text::String> s;
			UIntOS i;
			UInt32 ip;
			Int32 v;
			Text::PString sarr[2];
			if (cfg->GetValue(CSTR("DNS")).SetTo(s))
			{
				Data::ArrayListNative<UInt32> dnsList;
				Text::StringBuilderUTF8 sb;
				sb.Append(s);
				sarr[1] = sb;
				while (true)
				{
					i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
					ip = Net::SocketUtil::GetIPAddr(sarr[0].ToCString());
					if (ip)
					{
						dnsList.Add(ip);
					}
					if (i <= 1)
						break;
				}
				frm->SetDNSList(dnsList);
			}

			if (cfg->GetValue(CSTR("DisableV6")).SetTo(s) && s->ToInt32(v))
			{
				frm->SetDisableV6(v != 0);
			}

			if (cfg->GetValue(CSTR("Blacklist")).SetTo(s) && s->leng != 0)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(s);
				sarr[1] = sb;
				while (true)
				{
					i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
					frm->AddBlacklist(sarr[0].ToCString());
					if (i <= 1)
						break;
				}
			}

			cfg.Delete();
		}
		ui->Run();

		core.Delete();
		ui.Delete();
	}
	DEL_CLASS(exHdlr);
	return 0;
}
