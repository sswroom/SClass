#include "Stdafx.h"
#if defined(CPU_X86_32) ||  defined(CPU_X86_64)
#include "CPUInfoDetailx86.cpp"

#else
#include "Stdafx.h"
#include "Manage/CPUInfoDetail.h"

Manage::CPUInfoDetail::CPUInfoDetail()
{
	this->cpuModel = 0;
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

const UTF8Char *Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}
#endif
