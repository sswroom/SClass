#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileStream *fs;
	IO::ConsoleWriter console;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("SignalTestS.txt"));
	NEW_CLASS(fs, IO::FileStream({sbuff, (UOSInt)(sptr - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fs->Write((const UInt8*)"Start", 5);
	DEL_CLASS(fs);
	console.WriteLineC(UTF8STRC("SignalTest Started"));
	progCtrl->WaitForExit(progCtrl);

	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("SignalTestE.txt"));
	NEW_CLASS(fs, IO::FileStream({sbuff, (UOSInt)(sptr - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fs->Write((const UInt8*)"Ended", 5);
	DEL_CLASS(fs);
	return 0;
}
