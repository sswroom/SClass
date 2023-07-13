#include "Stdafx.h"
#include "IO/RobustelInfo.h"

Text::String *IO::RobustelInfo::GetIMEI()
{
	return Text::String::New(UTF8STRC("1234567890123456")).Ptr();
}

Text::String *IO::RobustelInfo::GetCellID()
{
	return Text::String::NewEmpty().Ptr();
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
	*lat = 0;
	*lon = 0;
	return true;
}
