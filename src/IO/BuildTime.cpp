#include "Stdafx.h"
#include "IO/BuildTime.h"

void IO::BuildTime::GetBuildTime(NN<Data::DateTime> dt)
{
	dt->SetTimeZoneQHR(Data::DateTimeUtil::GetLocalTzQhr());
	dt->SetValue(CSTR(__DATE__ " " __TIME__));
}

Data::Timestamp IO::BuildTime::GetBuildTime()
{
	return Data::Timestamp::FromStr(CSTR(__DATE__ " " __TIME__), Data::DateTimeUtil::GetLocalTzQhr());
}
