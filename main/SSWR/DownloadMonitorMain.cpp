#include "Stdafx.h"
#include "Core/Core.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "SSWR/DownloadMonitor/DownMonMainForm.h"
#include "UI/MessageDialog.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
#if 1
	Media::Decoder::FFMPEGDecoder::Enable();
#endif

	UI::GUICore *ui = progCtrl->CreateGUICore(progCtrl);
	if (ui)
	{
		SSWR::DownloadMonitor::DownMonCore *core;
		NEW_CLASS(core, SSWR::DownloadMonitor::DownMonCore());
		if (core->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in initializing the core", (const UTF8Char*)"Download Monitor", 0);
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
		DEL_CLASS(ui);
	}
	return 0;
}
