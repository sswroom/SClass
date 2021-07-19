#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPUManager.h"

IO::GPUManager::GPUManager()
{
	this->classObj = 0;
}

IO::GPUManager::~GPUManager()
{
}

UOSInt IO::GPUManager::GetGPUCount()
{
	return 0;
}

IO::IGPUControl *IO::GPUManager::GetGPUControl(UOSInt index)
{
	return 0;
}
