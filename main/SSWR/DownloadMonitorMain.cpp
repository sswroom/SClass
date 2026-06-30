#include "Stdafx.h"
#include "Core/Core.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "SSWR/DownloadMonitor/DownMonMainForm.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
#if 1
	Media::Decoder::FFMPEGDecoder::Enable();
#endif

	NN<UI::GUICore> ui;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		NN<SSWR::DownloadMonitor::DownMonCore> core;
		NEW_CLASSNN(core, SSWR::DownloadMonitor::DownMonCore());
		if (core->IsError())
		{
			ui->ShowMsgOK(CSTR("Error in initializing the core"), CSTR("Download Monitor"), nullptr);
		}
		else
		{
			NN<SSWR::DownloadMonitor::DownMonMainForm> frm;
			NEW_CLASSNN(frm, SSWR::DownloadMonitor::DownMonMainForm(nullptr, ui, core));
			frm->SetExitOnClose(true);
			frm->Show();
			ui->Run();
		}
		core.Delete();
		ui.Delete();
	}
	return 0;
}
