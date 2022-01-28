#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUInfoDetail.h"

Manage::CPUInfoDetail::CPUInfoDetail()
{
	this->cpuModel = CSTR("ATmega328P");
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

Text::CString Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}
