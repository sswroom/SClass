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
		this->name = Text::String::NewNotNull(sname);
	}
}

IO::SensorWin::~SensorWin()
{
	ISensor *pSensor = (ISensor*)this->sensor;
	pSensor->Release();
	SDEL_STRING(this->name);
}

Text::CString IO::SensorWin::GetVendor()
{
	return CSTR_NULL;
}

Text::CString IO::SensorWin::GetName()
{
	return STR_CSTR(this->name);
}

Bool IO::SensorWin::EnableSensor()
{
	return true;
}

Bool IO::SensorWin::DisableSensor()
{
	return true;
}

