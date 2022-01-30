#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/AMDGPUManager.h"
#include "IO/GPUManager.h"

struct IO::GPUManager::ClassData
{
	IO::AMDGPUManager *amdGPUMgr;
	Data::ArrayList<IO::IGPUControl *> *gpuList;
};

IO::GPUManager::GPUManager()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->amdGPUMgr, IO::AMDGPUManager());
	NEW_CLASS(clsData->gpuList, Data::ArrayList<IO::IGPUControl*>());
	this->clsData = clsData;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = clsData->amdGPUMgr->GetGPUCount();
	while (i < j)
	{
		clsData->gpuList->Add(clsData->amdGPUMgr->CreateGPUControl(i));
		i++;
	}
}

IO::GPUManager::~GPUManager()
{
	IO::IGPUControl *gpu;
	UOSInt i;
	i = this->clsData->gpuList->GetCount();
	while (i-- > 0)
	{
		gpu = this->clsData->gpuList->GetItem(i);
		DEL_CLASS(gpu);
	}
	DEL_CLASS(this->clsData->gpuList);
	DEL_CLASS(this->clsData->amdGPUMgr);
	MemFree(this->clsData);
}

UOSInt IO::GPUManager::GetGPUCount()
{
	return this->clsData->gpuList->GetCount();
}

IO::IGPUControl *IO::GPUManager::GetGPUControl(UOSInt index)
{
	return this->clsData->gpuList->GetItem(index);
}
