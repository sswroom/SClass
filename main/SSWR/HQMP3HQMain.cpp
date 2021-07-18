#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Media/Decoder/IMSDKDecoder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRHQMPForm.h"
#include "UI/GUICore.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	UI::GUICore *ui;
	SSWR::AVIRead::AVIRHQMPForm *frm;
	SSWR::AVIRead::AVIRCore *core;

//	MemSetBreakPoint(0x01088d78);
	Media::Decoder::FFMPEGDecoder::Enable();
#if defined(_MSC_VER)
	Media::Decoder::IMSDKDecoder::Enable();
#endif

	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"Error.log", Manage::ExceptionRecorder::EA_CLOSE));
	ui = progCtrl->CreateGUICore(progCtrl);
	NEW_CLASS(core, SSWR::AVIRead::AVIRCoreWin(ui));
	NEW_CLASS(frm, SSWR::AVIRead::AVIRHQMPForm(0, ui, core, SSWR::AVIRead::AVIRHQMPForm::QM_HQ));
	frm->SetExitOnClose(true);

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	while (argc-- > 1)
	{
		Bool succ = frm->OpenFile(argv[argc]);
		if (succ)
		{
			break;
		}
	}

	frm->Show();
	ui->Run();
	DEL_CLASS(core);
	DEL_CLASS(ui);
	DEL_CLASS(exHdlr);
	return 0;
}