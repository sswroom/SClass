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
	OSInt i;
	OSInt j;
	OSInt baseAddr;
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
	OSInt i = this->modNames->GetCount();
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
	while ((*buff++ = *cptr++) != 0);
	free(name);
	return buff - 1;
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

