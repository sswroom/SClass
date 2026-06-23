#include "Stdafx.h"
#include "IO/OS.h"
#include "Text/MyString.h"
#include <sys/sysctl.h>

UnsafeArrayOpt<UTF8Char> IO::OS::GetDistro(UnsafeArray<UTF8Char> sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("Darwin"));
}

UnsafeArrayOpt<UTF8Char> IO::OS::GetVersion(UnsafeArray<UTF8Char> sbuff)
{
	size_t len = 256;
	sysctlbyname("kern.osrelease", sbuff, &len, 0, 0);
	return &sbuff[len];
}
