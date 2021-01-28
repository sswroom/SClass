#include "stdafx.h"
#include "MyMemory.h"
#include "IO/Battery.h"
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#if defined(MSC_VER)
#include <BatClass.h>

#ifndef GUID_DEVCLASS_BATTERY
#define GUID_DEVCLASS_BATTERY GUID_DEVICE_BATTERY
#endif

IO::Battery::Battery(void *hand)
{
	this->hand = hand;
}

IO::Battery::~Battery()
{
	CloseHandle((HANDLE)hand);
}

Int32 IO::Battery::GetBatteryCount()
{
	HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (INVALID_HANDLE_VALUE != hdev)
	{
		SP_DEVICE_INTERFACE_DATA did;
		Int32 i;
		i = 0;
		while (i < 100)
		{
			did.cbSize = sizeof(did);
			if (SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVCLASS_BATTERY, i, &did))
			{
			}
			else if (GetLastError() == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			i++;
		}
		SetupDiDestroyDeviceInfoList(hdev);
		return i;
	}
	else
	{
		return 0;
	}
}

IO::Battery *IO::Battery::GetBattery(Int32 index)
{
	///////////////////////////////////////
	return 0;
}

IO::Battery::ACPowerStatus IO::Battery::GetACLineStatus()
{
	SYSTEM_POWER_STATUS status;
	if (GetSystemPowerStatus(&status))
	{
		return (IO::Battery::ACPowerStatus)status.ACLineStatus;
	}
	else
	{
		return IO::Battery::ACPWR_UNK;
	}
}

IO::Battery::BatteryFlag IO::Battery::GetBatteryFlag()
{
	SYSTEM_POWER_STATUS status;
	if (GetSystemPowerStatus(&status))
	{
		return (IO::Battery::BatteryFlag)status.BatteryFlag;
	}
	else
	{
		return IO::Battery::BATTFLG_UNK;
	}
}

Int32 IO::Battery::GetBatteryPercent()
{
	SYSTEM_POWER_STATUS status;
	if (GetSystemPowerStatus(&status))
	{
		return status.BatteryLifePercent;
	}
	else
	{
		return 255;
	}
}

Int32 IO::Battery::GetBatteryTimeSecond()
{
	SYSTEM_POWER_STATUS status;
	if (GetSystemPowerStatus(&status))
	{
		return status.BatteryLifeTime;
	}
	else
	{
		return -1;
	}
}

Int32 IO::Battery::GetBatteryChargeTimeSecond()
{
	SYSTEM_POWER_STATUS status;
	if (GetSystemPowerStatus(&status))
	{
		return status.BatteryFullLifeTime;
	}
	else
	{
		return -1;
	}
}
#endif
