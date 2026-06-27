#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/ModuleInfo.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

typedef struct
{
	UnsafeArray<const UTF8Char> fileName;
	UIntOS addr;
	UIntOS size;
} ModuleInfoData;

Manage::ModuleInfo::ModuleInfo(void *hProc, void *hMod)
{
	NN<ModuleInfoData> info;
	ModuleInfoData *input = (ModuleInfoData*)hMod;

	this->hProc = hProc;
	info = MemAllocNN(ModuleInfoData);
	this->hMod = info.Ptr();
	info->addr = input->addr;
	info->fileName = Text::StrCopyNew(input->fileName);
	info->size = input->size;
}

Manage::ModuleInfo::~ModuleInfo()
{
	NN<ModuleInfoData> info;
	if (info.Set((ModuleInfoData*)this->hMod))
	{
		Text::StrDelNew(info->fileName);
		MemFreeNN(info);
	}
}

UnsafeArray<UTF8Char> Manage::ModuleInfo::GetModuleFileName(UnsafeArray<UTF8Char> buff)
{
	ModuleInfoData *info = (ModuleInfoData*)this->hMod;
	return Text::StrConcat(buff, info->fileName);
}

Bool Manage::ModuleInfo::GetModuleAddress(OutParam<UIntOS> baseAddr, OutParam<UIntOS> size)
{
	ModuleInfoData *info = (ModuleInfoData*)this->hMod;
	baseAddr.Set(info->addr);
	size.Set(info->size);
	return true;
}

