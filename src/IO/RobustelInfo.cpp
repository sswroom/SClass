#include "Stdafx.h"
#include "IO/RobustelInfo.h"
#include "IO/RobustelStatus.h"

Text::String *IO::RobustelInfo::GetCellID()
{
	return 0;
}

Bool IO::RobustelInfo::GetRSSI(Int8 *val)
{
	return false;
}

Bool IO::RobustelInfo::GetPosition(Double *lat, Double *lon)
{
	return false;
}
