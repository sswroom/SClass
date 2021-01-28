#include "Stdafx.h"
#include "IO/OS.h"
#include "Text/MyString.h"
#include <sys/sysctl.h>

UTF8Char *IO::OS::GetDistro(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"Darwin");
}

UTF8Char *IO::OS::GetVersion(UTF8Char *sbuff)
{
	size_t len = 256;
	sysctlbyname("kern.osrelease", sbuff, &len, 0, 0);
	return &sbuff[len];
}
