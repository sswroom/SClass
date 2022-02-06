#include "Stdafx.h"
#include "IO/BuildTime.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"


void IO::BuildTime::GetBuildTime(Data::DateTime *dt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	IO::EXEFile::GetFileTime(CSTRP(sbuff, sptr), dt);
}
