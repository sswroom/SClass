#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/SymbolResolver.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Manage::SymbolResolver::SymbolResolver(NN<Manage::Process> proc)
{
	UIntOS i;
	UIntOS j;
	UIntOS baseAddr;
	UIntOS size;
	UTF8Char sbuff[256];
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
}

Manage::SymbolResolver::~SymbolResolver()
{
	this->modNames.FreeAll();
}

UnsafeArrayOpt<UTF8Char> Manage::SymbolResolver::ResolveName(UnsafeArray<UTF8Char> buff, UInt64 address)
{
	return nullptr;
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

