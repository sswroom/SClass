#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Manage/SymbolResolver.h"
#include <windows.h>
#include <stdio.h>
#undef GetModuleFileName

Manage::SymbolResolver::SymbolResolver(NN<Manage::Process> proc)
{
	UIntOS i;
	UIntOS j;
	UIntOS baseAddr;
	UIntOS size;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->proc = proc;

	Data::ArrayListNN<Manage::ModuleInfo> modList;
	this->proc->GetModules(modList);
	i = 0;
	j = modList.GetCount();
	while (i < j)
	{
		NN<Manage::ModuleInfo> mod;
		mod = modList.GetItemNoCheck(i);

		sptr = mod->GetModuleFileName(sbuff);
		this->modNames.Add(Text::String::NewP(sbuff, sptr));
		mod->GetModuleAddress(baseAddr, size);
		this->modBaseAddrs.Add(baseAddr);
		this->modSizes.Add(size);

		mod.Delete();
		i++;
	}
	modList.DeleteAll();
}

Manage::SymbolResolver::~SymbolResolver()
{
	this->modNames.FreeAll();
}

UnsafeArrayOpt<UTF8Char> Manage::SymbolResolver::ResolveName(UnsafeArray<UTF8Char> buff, UInt64 address)
{
	UIntOS i;
	UIntOS j;
	Bool found = false;
	NN<Text::String> name;

	i = this->modNames.GetCount();
	while (i-- > 0)
	{
		if (((UInt64)address) >= (UInt64)this->modBaseAddrs.GetItem(i) && ((UInt64)address) < (UInt64)(this->modBaseAddrs.GetItem(i) + this->modSizes.GetItem(i)))
		{
			name = this->modNames.GetItemNoCheck(i);
			j = name->LastIndexOf('\\');
			buff = name->Substring(j + 1).ConcatTo(buff);
			found = true;
			break;
		}
	}

	if (found)
	{
		if (address != this->modBaseAddrs.GetItem(i))
		{
			buff = Text::StrConcatC(buff, UTF8STRC("+0x"));
			buff = Text::StrHexVal32(buff, (UInt32)address - (UInt32)this->modBaseAddrs.GetItem(i));
		}
	}

	if (!found)
	{
		*buff = 0;
		return nullptr;
	}
	else
	{
		return buff;
	}
}

UIntOS Manage::SymbolResolver::GetModuleCount()
{
	return this->modNames.GetCount();
}

Optional<Text::String> Manage::SymbolResolver::GetModuleName(UIntOS index)
{
	return this->modNames.GetItem(index);
}

UInt64 Manage::SymbolResolver::GetModuleAddr(UIntOS index)
{
	return this->modBaseAddrs.GetItem(index);
}

UInt64 Manage::SymbolResolver::GetModuleSize(UIntOS index)
{
	return this->modSizes.GetItem(index);
}
