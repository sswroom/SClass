#include "stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Manage/SymbolResolver.h"
#include <windows.h>
#include <stdio.h>
#undef GetModuleFileName

Manage::SymbolResolver::SymbolResolver(Manage::Process *proc)
{
	OSInt i;
	OSInt j;
	OSInt baseAddr;
	UInt32 size;
	UTF8Char sbuff[512];
	NEW_CLASS(this->modNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->modBaseAddrs, Data::ArrayListInt64());
	NEW_CLASS(this->modSizes, Data::ArrayListInt32());
	this->proc = proc;

	Data::ArrayList<Manage::ModuleInfo*> *modList;
	NEW_CLASS(modList, Data::ArrayList<Manage::ModuleInfo*>());
	this->proc->GetModules(modList);
	i = 0;
	j = modList->GetCount();
	while (i < j)
	{
		Manage::ModuleInfo *mod;
		mod = modList->GetItem(i);

		mod->GetModuleFileName(sbuff);
		this->modNames->Add(Text::StrCopyNew(sbuff));
		mod->GetModuleAddress(&baseAddr, &size);
		this->modBaseAddrs->Add(baseAddr);
		this->modSizes->Add(size);

		DEL_CLASS(mod);
		i++;
	}
	DEL_CLASS(modList);
}

Manage::SymbolResolver::~SymbolResolver()
{
	DEL_CLASS(this->modSizes);
	DEL_CLASS(this->modBaseAddrs);
	OSInt i = this->modNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->modNames->GetItem(i));
	}
	DEL_CLASS(this->modNames);
}

UTF8Char *Manage::SymbolResolver::ResolveName(UTF8Char *buff, UInt64 address)
{
	OSInt i;
	OSInt j;
	Bool found = false;
	const UTF8Char *name;

	i = this->modNames->GetCount();
	while (i-- > 0)
	{
		if (((UInt64)address) >= (UInt64)this->modBaseAddrs->GetItem(i) && ((UInt64)address) < (UInt64)(this->modBaseAddrs->GetItem(i) + this->modSizes->GetItem(i)))
		{
			name = this->modNames->GetItem(i);
			j = Text::StrLastIndexOf(name, '\\');
			buff = Text::StrConcat(buff, &name[j + 1]);
			found = true;
			break;
		}
	}

	if (found)
	{
		if (address != this->modBaseAddrs->GetItem(i))
		{
			buff = Text::StrConcat(buff, (const UTF8Char*)"+0x");;
			buff = Text::StrHexVal32(buff, (UInt32)address - (UInt32)this->modBaseAddrs->GetItem(i));
		}
	}

	if (!found)
	{
		*buff = 0;
		return 0;
	}
	else
	{
		return buff;
	}
}

OSInt Manage::SymbolResolver::GetModuleCount()
{
	return this->modNames->GetCount();
}

const UTF8Char *Manage::SymbolResolver::GetModuleName(OSInt index)
{
	return this->modNames->GetItem(index);
}

Int64 Manage::SymbolResolver::GetModuleAddr(OSInt index)
{
	return this->modBaseAddrs->GetItem(index);
}

Int32 Manage::SymbolResolver::GetModuleSize(OSInt index)
{
	return this->modSizes->GetItem(index);
}
