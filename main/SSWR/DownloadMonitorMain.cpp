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
		SSWR::DownloadMonitor::DownMonCore *core;
		NEW_CLASS(core, SSWR::DownloadMonitor::DownMonCore());
		if (core->IsError())
		{
			ui->ShowMsgOK(CSTR("Error in initializing the core"), CSTR("Download Monitor"), 0);
		}
		else
		{
			SSWR::DownloadMonitor::DownMonMainForm *frm;
			NEW_CLASS(frm, SSWR::DownloadMonitor::DownMonMainForm(0, ui, core));
			frm->SetExitOnClose(true);
			frm->Show();
			ui->Run();
		}
		DEL_CLASS(core);
		ui.Delete();
	}
	return 0;
}
