#include "Stdafx.h"
#include "IO/SystemInfo.h"
#include "Text/MyString.h"

IO::SystemInfo::SystemInfo()
{
}

IO::SystemInfo::~SystemInfo()
{
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("Arduino"));
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
{
	return nullptr;
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

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

