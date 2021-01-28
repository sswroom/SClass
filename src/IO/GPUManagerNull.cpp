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

OSInt IO::GPUManager::GetGPUCount()
{
	return 0;
}

IO::IGPUControl *IO::GPUManager::GetGPUControl(OSInt index)
{
	return 0;
}
