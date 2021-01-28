#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUInfoDetail.h"

Manage::CPUInfoDetail::CPUInfoDetail()
{
	this->cpuModel = (const UTF8Char*)"ATmega328P";
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

const UTF8Char *Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}
