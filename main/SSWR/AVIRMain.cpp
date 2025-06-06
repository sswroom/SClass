#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#if 1
#include "Media/Decoder/FFMPEGDecoder.h"
#define USE_FFMPEG
#endif
#if 0 && defined(WIN32) && defined(CPU_X86_64)
//#include "Media/Decoder/IMSDKDecoder.h"
#define USE_IMSDK
#endif
#if 0 && !defined(WIN32)
//#include "Media/Decoder/VPXDecoder.h"
#define USE_VPX
#endif
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRBaseForm.h"

Int32 MyAdd(Int32 a, Int32 b)
{
	Int32 tmp;
	while (b != 0)
	{
		tmp = a & b;
		a = a ^ b;
		b = tmp << 1;
	}
	return a;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	SSWR::AVIRead::AVIRBaseForm *frm;
	NN<SSWR::AVIRead::AVIRCore> core;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt argc;
	UOSInt i;
	UnsafeArray<UnsafeArray<UTF8Char>> argv;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile(UTF8STRCPTR("Memory.log"));
#ifdef USE_FFMPEG
	Media::Decoder::FFMPEGDecoder::Enable();
#endif
#ifdef USE_IMSDK
	Media::Decoder::IMSDKDecoder::Enable();
#endif
#ifdef USE_VPX
	Media::Decoder::VPXDecoder::Enable();
#endif

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("log"));
	Manage::ExceptionRecorder exHdlr(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_CLOSE);
	if (Core::ProgControl::CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASS(frm, SSWR::AVIRead::AVIRBaseForm(0, ui, core));
		frm->SetExitOnClose(true);
		frm->Show();
		argv = progCtrl->GetCommandLines(progCtrl, argc);
		if (argc > 1)
		{
			core->BeginLoad();
			i = 1;
			while (i < argc)
			{
				IO::StmData::FileData fd({argv[i], Text::StrCharCnt(argv[i])}, false);
				core->LoadData(fd, 0);

				i++;
			}
			core->EndLoad();
		}
		ui->Run();

		core.Delete();
		ui.Delete();
	}
	return 0;
}
