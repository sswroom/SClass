#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Media/MMALStillCapture.h"
#include "Sync/ThreadUtil.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Media::MMALStillCapture capture;
	if (capture.DeviceBegin())
	{
		Media::PhotoCapturer::PhotoFormat pf;
		Sync::SimpleThread::Sleep(5000);
		{
			IO::FileStream fs(CSTR("Capture.jpg"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!capture.CapturePhoto(&pf, &fs))
			{
				console.WriteLine(CSTR("Error in capturing image"));
			}
			else
			{
				console.WriteLine(CSTR("Image captured to Capture.jpg"));
			}
		}

		Sync::SimpleThread::Sleep(5000);
		{
			IO::FileStream fs(CSTR("Capture2.jpg"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!capture.CapturePhoto(&pf, &fs))
			{
				console.WriteLine(CSTR("Error in capturing image"));
			}
			else
			{
				console.WriteLine(CSTR("Image captured to Capture2.jpg"));
			}
		}
		capture.DeviceEnd();
	}
	else
	{
		console.WriteLine(CSTR("Error in begin capture"));
	}
	return 0;
}
