#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorWin.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <sensorsapi.h>
#include <sensors.h>

IO::SensorWin::SensorWin(void *sensor)
{
	BSTR sname;
	this->name = 0;
	this->sensor = sensor;
	ISensor *pSensor = (ISensor*)this->sensor;
	pSensor->SetEventSink(0);
	if (SUCCEEDED(pSensor->GetFriendlyName(&sname)))
	{
		this->name = Text::StrToUTF8New(sname);
	}
}

IO::SensorWin::~SensorWin()
{
	ISensor *pSensor = (ISensor*)this->sensor;
	pSensor->Release();
	SDEL_TEXT(this->name);
}

const UTF8Char *IO::SensorWin::GetVendor()
{
	return 0;
}

const UTF8Char *IO::SensorWin::GetName()
{
	return this->name;
}

Bool IO::SensorWin::EnableSensor()
{
	return true;
}

Bool IO::SensorWin::DisableSensor()
{
	return true;
}

