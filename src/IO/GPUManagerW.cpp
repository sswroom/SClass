#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNN.hpp"
#include "IO/AMDGPUManager.h"
#include "IO/GPUManager.h"

struct IO::GPUManager::ClassData
{
	NN<IO::AMDGPUManager> amdGPUMgr;
	NN<Data::ArrayListNN<IO::GPUControl>> gpuList;
};

IO::GPUManager::GPUManager()
{
	NN<ClassData> clsData = MemAllocNN(ClassData);
	NEW_CLASSNN(clsData->amdGPUMgr, IO::AMDGPUManager());
	NEW_CLASSNN(clsData->gpuList, Data::ArrayListNN<IO::GPUControl>());
	NN<IO::GPUControl> ctrl;
	this->clsData = clsData;
	UIntOS i;
	UIntOS j;
	i = 0;
	j = clsData->amdGPUMgr->GetGPUCount();
	while (i < j)
	{
		if (clsData->amdGPUMgr->CreateGPUControl(i).SetTo(ctrl))
			clsData->gpuList->Add(ctrl);
		i++;
	}
}

IO::GPUManager::~GPUManager()
{
	NN<IO::GPUControl> gpu;
	UIntOS i;
	i = this->clsData->gpuList->GetCount();
	while (i-- > 0)
	{
		gpu = this->clsData->gpuList->GetItemNoCheck(i);
		gpu.Delete();
	}
	this->clsData->gpuList.Delete();
	this->clsData->amdGPUMgr.Delete();
	MemFreeNN(this->clsData);
}

UIntOS IO::GPUManager::GetGPUCount()
{
	return this->clsData->gpuList->GetCount();
}

Optional<IO::GPUControl> IO::GPUManager::GetGPUControl(UIntOS index)
{
	return this->clsData->gpuList->GetItem(index);
}
