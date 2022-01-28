#include "Stdafx.h"
#if defined(CPU_X86_32) ||  defined(CPU_X86_64)
#include "CPUInfoDetailx86.cpp"

#else
#include "Stdafx.h"
#include "Manage/CPUInfoDetail.h"

Manage::CPUInfoDetail::CPUInfoDetail()
{
	this->cpuModel = CSTR_NULL;
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

Text::CString Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}

Bool Manage::CPUInfoDetail::GetCPUTemp(UOSInt index, Double *temp)
{
	return false;
}
#endif
