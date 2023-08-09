#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/PowerInfo.h"

Bool Power_ReadIntFile(Text::CStringNN filePath, OutParam<Int32> val)
{
	UInt8 buff[128];
	UOSInt readSize;
	Bool succ = false;
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		readSize = fs.Read(BYTEARR(buff).WithSize(127));
		if (readSize > 0)
		{
			buff[readSize] = 0;
			if (buff[readSize - 1] == 13)
			{
				buff[readSize - 1] = 0;
			}
			else if (buff[readSize - 1] == 10)
			{
				if (buff[readSize - 2] == 13)
				{
					buff[readSize - 2] = 0;
				}
				else
				{
					buff[readSize - 1] = 0;
				}
			}
			succ = Text::StrToInt32((UTF8Char*)buff, val);
		}
	}
	return succ;
}

Bool Power_ReadStrFile(Text::CStringNN filePath, UTF8Char *val, UOSInt maxCharCnt)
{
	UOSInt readSize;
	Bool succ = false;
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		readSize = fs.Read(Data::ByteArray(val, maxCharCnt));
		if (readSize > 0)
		{
			val[readSize] = 0;
			if (val[readSize - 1] == 13)
			{
				val[readSize - 1] = 0;
			}
			else if (val[readSize - 1] == 10)
			{
				if (val[readSize - 2] == 13)
				{
					val[readSize - 2] = 0;
				}
				else
				{
					val[readSize - 1] = 0;
				}
			}
			succ = true;
		}
	}
	return succ;
}

Bool IO::PowerInfo::GetPowerStatus(PowerStatus *power)
{
	UTF8Char cbuff[128];
	Int32 ival;
	MemClear(power, sizeof(PowerStatus));
	if (Power_ReadIntFile(CSTR("/sys/class/power_supply/AC/online"), ival))
	{
		if (ival == 1)
		{
			power->acStatus = ACS_ON;
		}
		else
		{
			power->acStatus = ACS_OFF;
		}
	}
	else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/ac/online"), ival))
	{
		if (ival == 1)
		{
			power->acStatus = ACS_ON;
		}
		else
		{
			power->acStatus = ACS_OFF;
		}
	}
	else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/rk-ac/online"), ival))
	{
		if (ival == 1)
		{
			power->acStatus = ACS_ON;
		}
		else
		{
			power->acStatus = ACS_OFF;
		}
	}
	else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/usb/online"), ival))
	{
		if (ival == 1)
		{
			power->acStatus = ACS_ON;
		}
		else
		{
			power->acStatus = ACS_OFF;
		}
	}
	else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/ACAD/online"), ival))
	{
		if (ival == 1)
		{
			power->acStatus = ACS_ON;
		}
		else
		{
			power->acStatus = ACS_OFF;
		}
	}
	else
	{
		power->acStatus = ACS_UNKNOWN;
	}
	if (Power_ReadStrFile(CSTR("/sys/class/power_supply/battery/status"), cbuff, 127))
	{
		power->hasBattery = true;
		power->batteryCharging = Text::StrEquals(cbuff, (const UTF8Char*)"Charging");
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/capacity"), ival))
		{
			power->batteryPercent = (UInt32)ival;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/voltage_now"), ival))
		{
			power->batteryVoltage = ival * 0.000001;
		}
		else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/batt_vol"), ival))
		{
			power->batteryVoltage = ival * 0.001;
		}

		power->batteryChargeCurrent = 0;
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/current_now"), ival))
		{
			power->batteryChargeCurrent -= ival * 0.000001;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/input_current_now"), ival))
		{
			power->batteryChargeCurrent += ival * 0.000001;
		}
		else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/chg_current"), ival))
		{
			power->batteryChargeCurrent += ival * 0.001;
		}

		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/temp"), ival))
		{
			power->batteryTemp = ival * 0.1;
		}
		else if (Power_ReadIntFile(CSTR("/sys/class/power_supply/battery/batt_temp"), ival))
		{
			power->batteryTemp = ival * 0.1;
		}
	}
	else if (Power_ReadStrFile(CSTR("/sys/class/power_supply/BAT0/status"), cbuff, 127))
	{
		power->batteryCharging = Text::StrEquals(cbuff, (const UTF8Char*)"Charging");
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT0/present"), ival))
		{
			power->hasBattery = (ival != 0);
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT0/capacity"), ival))
		{
			power->batteryPercent = (UInt32)ival;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT0/voltage_now"), ival))
		{
			power->batteryVoltage = ival * 0.000001;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT0/current_now"), ival))
		{
			power->batteryChargeCurrent = ival * 0.000001;
		}
	}
	else if (Power_ReadStrFile(CSTR("/sys/class/power_supply/BAT1/status"), cbuff, 127))
	{
		power->batteryCharging = Text::StrEquals(cbuff, (const UTF8Char*)"Charging");
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT1/present"), ival))
		{
			power->hasBattery = (ival != 0);
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT1/capacity"), ival))
		{
			power->batteryPercent = (UInt32)ival;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT1/voltage_now"), ival))
		{
			power->batteryVoltage = ival * 0.000001;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/BAT1/current_now"), ival))
		{
			power->batteryChargeCurrent = ival * 0.000001;
		}
	}
	else if (Power_ReadStrFile(CSTR("/sys/class/power_supply/rk-bat/status"), cbuff, 127))
	{
		power->batteryCharging = Text::StrEquals(cbuff, (const UTF8Char*)"Charging");
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/rk-bat/present"), ival))
		{
			power->hasBattery = (ival != 0);
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/rk-bat/capacity"), ival))
		{
			power->batteryPercent = (UInt32)ival;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/rk-bat/voltage_now"), ival))
		{
			power->batteryVoltage = ival * 0.000001;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/rk-bat/current_now"), ival))
		{
			power->batteryChargeCurrent = ival * 0.000001;
		}
	}
	else if (Power_ReadStrFile(CSTR("/sys/class/power_supply/Battery/status"), cbuff, 127))
	{
		power->batteryCharging = Text::StrEquals(cbuff, (const UTF8Char*)"Charging");
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/Battery/present"), ival))
		{
			power->hasBattery = (ival != 0);
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/Battery/capacity"), ival))
		{
			power->batteryPercent = (UInt32)ival;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/Battery/voltage_now"), ival))
		{
			power->batteryVoltage = ival * 0.000001;
		}
		if (Power_ReadIntFile(CSTR("/sys/class/power_supply/Battery/current_now"), ival))
		{
			power->batteryChargeCurrent = ival * 0.000001;
		}
	}
	else
	{
		power->hasBattery = false;
		power->batteryCharging = false;
		power->batteryPercent = 0;
		power->timeLeftSec = 0;
	}
	return true;
}
