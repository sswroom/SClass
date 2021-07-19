#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/AMDGPUManager.h"
#include "IO/GPUManager.h"

typedef struct
{
	IO::AMDGPUManager *amdGPUMgr;
	Data::ArrayList<IO::IGPUControl *> *gpuList;
} ClassData;

IO::GPUManager::GPUManager()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->amdGPUMgr, IO::AMDGPUManager());
	NEW_CLASS(clsData->gpuList, Data::ArrayList<IO::IGPUControl*>());
	this->classObj = clsData;
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
	ClassData *clsData = (ClassData*)this->classObj;
	IO::IGPUControl *gpu;
	UOSInt i;
	i = clsData->gpuList->GetCount();
	while (i-- > 0)
	{
		gpu = clsData->gpuList->GetItem(i);
		DEL_CLASS(gpu);
	}
	DEL_CLASS(clsData->gpuList);
	DEL_CLASS(clsData->amdGPUMgr);
	MemFree(clsData);
}

UOSInt IO::GPUManager::GetGPUCount()
{
	ClassData *clsData = (ClassData*)this->classObj;
	return clsData->gpuList->GetCount();
}

IO::IGPUControl *IO::GPUManager::GetGPUControl(UOSInt index)
{
	ClassData *clsData = (ClassData*)this->classObj;
	return clsData->gpuList->GetItem(index);
}
