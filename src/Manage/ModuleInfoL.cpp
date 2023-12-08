#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/ModuleInfo.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

typedef struct
{
	const UTF8Char *fileName;
	UOSInt addr;
	UOSInt size;
} ModuleInfoData;

Manage::ModuleInfo::ModuleInfo(void *hProc, void *hMod)
{
	ModuleInfoData *info;
	ModuleInfoData *input = (ModuleInfoData*)hMod;

	this->hProc = hProc;
	info = MemAlloc(ModuleInfoData, 1);
	this->hMod = info;
	info->addr = input->addr;
	info->fileName = Text::StrCopyNew(input->fileName).Ptr();
	info->size = input->size;
}

Manage::ModuleInfo::~ModuleInfo()
{
	ModuleInfoData *info = (ModuleInfoData*)this->hMod;
	Text::StrDelNew(info->fileName);
	MemFree(info);
}

UTF8Char *Manage::ModuleInfo::GetModuleFileName(UTF8Char *buff)
{
	ModuleInfoData *info = (ModuleInfoData*)this->hMod;
	return Text::StrConcat(buff, info->fileName);
}

Bool Manage::ModuleInfo::GetModuleAddress(OutParam<UOSInt> baseAddr, OutParam<UOSInt> size)
{
	ModuleInfoData *info = (ModuleInfoData*)this->hMod;
	baseAddr.Set(info->addr);
	size.Set(info->size);
	return true;
}

