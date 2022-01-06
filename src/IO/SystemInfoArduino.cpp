#include "Stdafx.h"
#include "IO/SystemInfo.h"
#include "Text/MyString.h"

IO::SystemInfo::SystemInfo()
{
}

IO::SystemInfo::~SystemInfo()
{
}

UTF8Char *IO::SystemInfo::GetPlatformName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("Arduino"));
}

UTF8Char *IO::SystemInfo::GetPlatformSN(UTF8Char *sbuff)
{
	return 0;
}

UInt64 IO::SystemInfo::GetTotalMemSize()
{
	return 2048 * (UInt64)1024;
}

UInt64 IO::SystemInfo::GetTotalUsableMemSize()
{
	return 2048 * (UInt64)1024;
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

