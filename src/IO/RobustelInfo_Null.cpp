#include "Stdafx.h"
#include "IO/RobustelInfo.h"

Text::String *IO::RobustelInfo::GetIMEI()
{
	return Text::String::NewNotNull((const UTF8Char*)"1234567890123456");
}

Text::String *IO::RobustelInfo::GetCellID()
{
	return 0;
}

Bool IO::RobustelInfo::GetRSSI(Int8 *val)
{
	return false;
}

Bool IO::RobustelInfo::GetRSRP(Int8 *val)
{
	return false;
}

Bool IO::RobustelInfo::GetRSRQ(Int8 *val)
{
	return false;
}

Bool IO::RobustelInfo::GetPosition(Double *lat, Double *lon)
{
	return false;
}
