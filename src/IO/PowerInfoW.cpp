#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PowerInfo.h"
#include <windows.h>

Bool IO::PowerInfo::GetPowerStatus(PowerStatus *power)
{
	SYSTEM_POWER_STATUS status;
	if (GetSystemPowerStatus(&status) == 0)
		return false;

	MemClear(power, sizeof(PowerStatus));
	if (status.ACLineStatus == 0)
	{
		power->acStatus = ACS_OFF;
	}
	else if (status.ACLineStatus == 1)
	{
		power->acStatus = ACS_ON;
	}
	else
	{
		power->acStatus = ACS_UNKNOWN;
	}
	power->hasBattery = (status.BatteryFlag & 128) == 0;
	power->batteryCharging = (status.BatteryFlag & 8) != 0;
	power->batteryPercent = status.BatteryLifePercent;
	power->timeLeftSec = status.BatteryLifeTime;
	return true;
}
