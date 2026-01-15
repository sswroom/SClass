#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/SymbolResolver.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Manage::SymbolResolver::SymbolResolver(Manage::Process *proc)
{
	IntOS i;
	IntOS j;
	IntOS baseAddr;
	UInt32 size;
	UTF8Char sbuff[256];
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
	IntOS i = this->modNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->modNames->GetItem(i));
	}
	DEL_CLASS(this->modNames);
}

UTF8Char *Manage::SymbolResolver::ResolveName(UTF8Char *buff, UInt64 address)
{
	return 0;
}

IntOS Manage::SymbolResolver::GetModuleCount()
{
	return this->modNames->GetCount();
}

const UTF8Char *Manage::SymbolResolver::GetModuleName(IntOS index)
{
	return this->modNames->GetItem(index);
}

Int64 Manage::SymbolResolver::GetModuleAddr(IntOS index)
{
	return this->modBaseAddrs->GetItem(index);
}

Int32 Manage::SymbolResolver::GetModuleSize(IntOS index)
{
	return this->modSizes->GetItem(index);
}

