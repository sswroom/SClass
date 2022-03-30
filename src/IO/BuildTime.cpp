#include "Stdafx.h"
#include "IO/BuildTime.h"

void IO::BuildTime::GetBuildTime(Data::DateTime *dt)
{
	dt->SetTimeZoneQHR(0);
	dt->SetValue(CSTR(__DATE__ " " __TIME__));
}
