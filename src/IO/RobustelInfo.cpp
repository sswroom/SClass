#include "Stdafx.h"
#include "IO/RobustelInfo.h"
#include "IO/RobustelStatus.h"

Text::String *IO::RobustelInfo::GetCellID()
{
	return IO::RobustelStatus::GetStatus("cellular.status.cell_id");
}

Bool IO::RobustelInfo::GetRSSI(Int8 *val)
{
	Text::String *s = IO::RobustelStatus::GetStatus("cellular.status.csq");
	SDEL_STRING(s);
	return false;
}

Bool IO::RobustelInfo::GetRSRP(Int8 *val)
{
	Text::String *s = IO::RobustelStatus::GetStatus("cellular.status.rsrp");
	SDEL_STRING(s);
	return false;
}

Bool IO::RobustelInfo::GetRSRQ(Int8 *val)
{
	Text::String *s = IO::RobustelStatus::GetStatus("cellular.status.rsrq");
	SDEL_STRING(s);
	return false;
}

Bool IO::RobustelInfo::GetPosition(Double *lat, Double *lon)
{
	Text::String *s;
	s = IO::RobustelStatus::GetStatus("gps.latitude");
	SDEL_STRING(s);
	s = IO::RobustelStatus::GetStatus("gps.longitude");
	SDEL_STRING(s);
	return false;
}
