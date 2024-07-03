#include "Stdafx.h"
#include "IO/BuildTime.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"

void IO::BuildTime::GetBuildTime(NN<Data::DateTime> dt)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	IO::EXEFile::GetFileTime(CSTRP(sbuff, sptr), dt);
	dt->ToLocalTime();
}

Data::Timestamp IO::BuildTime::GetBuildTime()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	return IO::EXEFile::GetFileTime(CSTRP(sbuff, sptr)).ToLocalTime();

}
