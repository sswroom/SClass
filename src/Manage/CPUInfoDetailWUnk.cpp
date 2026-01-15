#include "Stdafx.h"
#if defined(CPU_X86_32) ||  defined(CPU_X86_64)
#include "CPUInfoDetailx86.cpp"

#else
#include "Stdafx.h"
#include "Manage/CPUInfoDetail.h"

Manage::CPUInfoDetail::CPUInfoDetail()
{
	this->cpuModel = nullptr;
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

Text::CString Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}

Bool Manage::CPUInfoDetail::GetCPUTemp(UIntOS index, OutParam<Double> temp)
{
	return false;
}
#endif
