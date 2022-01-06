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
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fs->Write((const UInt8*)"Start", 5);
	DEL_CLASS(fs);
	console.WriteLineC(UTF8STRC("SignalTest Started"));
	progCtrl->WaitForExit(progCtrl);

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"SignalTestE.txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fs->Write((const UInt8*)"Ended", 5);
	DEL_CLASS(fs);
	return 0;
}
