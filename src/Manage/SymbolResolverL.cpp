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

Manage::SymbolResolver::SymbolResolver(NN<Manage::Process> proc)
{
	UOSInt baseAddr;
	UOSInt size;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	this->proc = proc;

	Data::ArrayListNN<Manage::ModuleInfo> modList;
	this->proc->GetModules(modList);
	Data::ArrayIterator<NN<Manage::ModuleInfo>> it = modList.Iterator();
	while (it.HasNext())
	{
		NN<Manage::ModuleInfo> mod = it.Next();

		sptr = mod->GetModuleFileName(sbuff);
		this->modNames.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		mod->GetModuleAddress(baseAddr, size);
		this->modBaseAddrs.Add(baseAddr);
		this->modSizes.Add(size);

		mod.Delete();
	}
}

Manage::SymbolResolver::~SymbolResolver()
{
	UOSInt i = this->modNames.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->modNames.GetItem(i));
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

Optional<Text::String> Manage::SymbolResolver::GetModuleName(UOSInt index)
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

