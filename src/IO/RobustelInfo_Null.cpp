#include "Stdafx.h"
#include "IO/RobustelInfo.h"

Optional<Text::String> IO::RobustelInfo::GetIMEI()
{
	return Text::String::New(UTF8STRC("1234567890123456"));
}

Optional<Text::String> IO::RobustelInfo::GetCellID()
{
	return Text::String::NewEmpty();
}

Bool IO::RobustelInfo::GetRSSI(OutParam<Int8> val)
{
	return false;
}

Bool IO::RobustelInfo::GetRSRP(OutParam<Int8> val)
{
	return false;
}

Bool IO::RobustelInfo::GetRSRQ(OutParam<Int8> val)
{
	return false;
}

Bool IO::RobustelInfo::GetPosition(OutParam<Double> lat, OutParam<Double> lon)
{
	lat.Set(0);
	lon.Set(0);
	return true;
}
