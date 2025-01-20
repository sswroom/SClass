#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNN.h"
#include "IO/AMDGPUManager.h"
#include "IO/GPUManager.h"

struct IO::GPUManager::ClassData
{
	IO::AMDGPUManager *amdGPUMgr;
	Data::ArrayListNN<IO::GPUControl> *gpuList;
};

IO::GPUManager::GPUManager()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->amdGPUMgr, IO::AMDGPUManager());
	NEW_CLASS(clsData->gpuList, Data::ArrayListNN<IO::GPUControl>());
	NN<IO::GPUControl> ctrl;
	this->clsData = clsData;
	UOSInt i;
	UOSInt j;
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
	UOSInt i;
	i = this->clsData->gpuList->GetCount();
	while (i-- > 0)
	{
		gpu = this->clsData->gpuList->GetItemNoCheck(i);
		gpu.Delete();
	}
	DEL_CLASS(this->clsData->gpuList);
	DEL_CLASS(this->clsData->amdGPUMgr);
	MemFree(this->clsData);
}

UOSInt IO::GPUManager::GetGPUCount()
{
	return this->clsData->gpuList->GetCount();
}

Optional<IO::GPUControl> IO::GPUManager::GetGPUControl(UOSInt index)
{
	return this->clsData->gpuList->GetItem(index);
}
