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
	OSInt i;
	OSInt j;
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
	OSInt i;
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

OSInt IO::GPUManager::GetGPUCount()
{
	ClassData *clsData = (ClassData*)this->classObj;
	return clsData->gpuList->GetCount();
}

IO::IGPUControl *IO::GPUManager::GetGPUControl(OSInt index)
{
	ClassData *clsData = (ClassData*)this->classObj;
	return clsData->gpuList->GetItem(index);
}
