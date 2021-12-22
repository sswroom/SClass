#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Media/MMALStillCapture.h"
#include "Sync/Thread.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Media::MMALStillCapture *capture;
	IO::ConsoleWriter console;
	NEW_CLASS(capture, Media::MMALStillCapture());
	if (capture->DeviceBegin())
	{
		IO::FileStream *fs;
		Media::IPhotoCapture::PhotoFormat pf;
		Sync::Thread::Sleep(5000);
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"Capture.jpg", IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!capture->CapturePhoto(&pf, fs))
		{
			console.WriteLine((const UTF8Char*)"Error in capturing image");
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Image captured to Capture.jpg");
		}
		DEL_CLASS(fs);

		Sync::Thread::Sleep(5000);
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"Capture2.jpg", IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!capture->CapturePhoto(&pf, fs))
		{
			console.WriteLine((const UTF8Char*)"Error in capturing image");
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Image captured to Capture2.jpg");
		}
		DEL_CLASS(fs);
		capture->DeviceEnd();
	}
	else
	{
		console.WriteLine((const UTF8Char*)"Error in begin capture");
	}
	DEL_CLASS(capture);
	return 0;
}
