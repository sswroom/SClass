#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Media/Decoder/IMSDKDecoder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRHQMPForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	SSWR::AVIRead::AVIRHQMPForm *frm;

//	MemSetBreakPoint(0x01088d78);
	Media::Decoder::FFMPEGDecoder::Enable();
#if defined(_MSC_VER)
	Media::Decoder::IMSDKDecoder::Enable();
#endif

	Manage::ExceptionRecorder exHdlr(CSTR("Error.log"), Manage::ExceptionRecorder::EA_CLOSE);
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		SSWR::AVIRead::AVIRCoreWin core(ui);
		NEW_CLASS(frm, SSWR::AVIRead::AVIRHQMPForm(nullptr, ui, core, SSWR::AVIRead::AVIRHQMPForm::QM_HQ));
		frm->SetExitOnClose(true);

		UIntOS argc;
		UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
		while (argc-- > 1)
		{
			Bool succ = frm->OpenFile({argv[argc], Text::StrCharCnt(argv[argc])}, IO::ParserType::MediaFile);
			if (succ)
			{
				break;
			}
		}

		frm->Show();
		ui->Run();
		ui.Delete();
	}
	return 0;
}