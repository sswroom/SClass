#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/SymbolResolver.h"
#include <stdlib.h>
#if defined(NO_STACKTRACE) || defined(__ANDROID__)
char **backtrace_symbols(void **addrArr, int cnt)
{
	char *val;
	char **ret = (char**)malloc(cnt * sizeof(char*) + 1);
	val = (char*)&ret[cnt];
	*val = 0;
	while (cnt-- > 0)
	{
		ret[cnt] = val;
	}
	return ret;
}
#else
#include <execinfo.h>
#endif

Manage::SymbolResolver::SymbolResolver(Manage::Process *proc)
{
	UOSInt i;
	UOSInt j;
	UOSInt baseAddr;
	UInt32 size;
	UTF8Char sbuff[256];
	NEW_CLASS(this->modNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->modBaseAddrs, Data::ArrayListUInt64());
	NEW_CLASS(this->modSizes, Data::ArrayListUInt32());
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
	UOSInt i = this->modNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->modNames->GetItem(i));
	}
	DEL_CLASS(this->modNames);
}

UTF8Char *Manage::SymbolResolver::ResolveName(UTF8Char *buff, UInt64 address)
{
	void *addr = (void*)(OSInt)address;
	char **name = backtrace_symbols(&addr, 1);
	char *cptr = name[0];
	while ((*buff++ = (UTF8Char)*cptr++) != 0);
	free(name);
	return buff - 1;
}

UOSInt Manage::SymbolResolver::GetModuleCount()
{
	return this->modNames->GetCount();
}

const UTF8Char *Manage::SymbolResolver::GetModuleName(UOSInt index)
{
	return this->modNames->GetItem(index);
}

UInt64 Manage::SymbolResolver::GetModuleAddr(UOSInt index)
{
	return this->modBaseAddrs->GetItem(index);
}

UInt32 Manage::SymbolResolver::GetModuleSize(UOSInt index)
{
	return this->modSizes->GetItem(index);
}

