#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/SymbolResolver.h"
#include <stdlib.h>
#if defined(NO_STACKTRACE) || defined(__ANDROID__)
char **backtrace_symbols(void **addrArr, int cnt)
{
	char *val;
	char **ret = (char**)malloc((unsigned long)cnt * sizeof(char*) + 1);
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

Manage::SymbolResolver::SymbolResolver(NotNullPtr<Manage::Process> proc)
{
	UOSInt i;
	UOSInt j;
	UOSInt baseAddr;
	UOSInt size;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	this->proc = proc;

	Data::ArrayListNN<Manage::ModuleInfo> modList;
	this->proc->GetModules(modList);
	i = 0;
	j = modList.GetCount();
	while (i < j)
	{
		Manage::ModuleInfo *mod;
		mod = modList.GetItem(i);

		sptr = mod->GetModuleFileName(sbuff);
		this->modNames.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		mod->GetModuleAddress(baseAddr, size);
		this->modBaseAddrs.Add(baseAddr);
		this->modSizes.Add(size);

		DEL_CLASS(mod);
		i++;
	}
}

Manage::SymbolResolver::~SymbolResolver()
{
	UOSInt i = this->modNames.GetCount();
	while (i-- > 0)
	{
		this->modNames.GetItem(i)->Release();
	}
}

UTF8Char *Manage::SymbolResolver::ResolveName(UTF8Char *buff, UInt64 address)
{
	void *addr = (void*)(OSInt)address;
	char **name = backtrace_symbols(&addr, 1);
	if (name)
	{
		buff = Text::StrConcat(buff, (UTF8Char*)name[0]);
		free(name);
		return buff;
	}
	return 0;
}

UOSInt Manage::SymbolResolver::GetModuleCount()
{
	return this->modNames.GetCount();
}

Text::String *Manage::SymbolResolver::GetModuleName(UOSInt index)
{
	return this->modNames.GetItem(index);
}

UInt64 Manage::SymbolResolver::GetModuleAddr(UOSInt index)
{
	return this->modBaseAddrs.GetItem(index);
}

UInt64 Manage::SymbolResolver::GetModuleSize(UOSInt index)
{
	return this->modSizes.GetItem(index);
}

