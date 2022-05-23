#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Media/MMALStillCapture.h"
#include "Sync/Thread.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Media::MMALStillCapture capture;
	if (capture.DeviceBegin())
	{
		Media::IPhotoCapture::PhotoFormat pf;
		Sync::Thread::Sleep(5000);
		{
			IO::FileStream fs(CSTR("Capture.jpg"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!capture.CapturePhoto(&pf, &fs))
			{
				console.WriteLineC(UTF8STRC("Error in capturing image"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("Image captured to Capture.jpg"));
			}
		}

		Sync::Thread::Sleep(5000);
		{
			IO::FileStream fs(CSTR("Capture2.jpg"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!capture.CapturePhoto(&pf, &fs))
			{
				console.WriteLineC(UTF8STRC("Error in capturing image"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("Image captured to Capture2.jpg"));
			}
		}
		capture.DeviceEnd();
	}
	else
	{
		console.WriteLineC(UTF8STRC("Error in begin capture"));
	}
	return 0;
}
