#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[512];
	IO::FileStream *fs;
	IO::ConsoleWriter console;
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"SignalTestS.txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	fs->Write((const UInt8*)"Start", 5);
	DEL_CLASS(fs);
	console.WriteLine((const UTF8Char*)"SignalTest Started");
	progCtrl->WaitForExit(progCtrl);

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"SignalTestE.txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	fs->Write((const UInt8*)"Ended", 5);
	DEL_CLASS(fs);
	return 0;
}
