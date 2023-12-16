#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Manage/SymbolResolver.h"
#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <stdio.h>
#undef GetModuleFileName

Manage::SymbolResolver::SymbolResolver(NotNullPtr<Manage::Process> proc)
{
	UOSInt baseAddr;
	UOSInt size;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	this->proc = proc;
	SymInitialize((HANDLE)this->proc->GetHandle(), 0, TRUE);

	Data::ArrayListNN<Manage::ModuleInfo> modList;
	this->proc->GetModules(modList);
	Data::ArrayIterator<NotNullPtr<Manage::ModuleInfo>> it = modList.Iterator();
	while (it.HasNext())
	{
		NotNullPtr<Manage::ModuleInfo> mod = it.Next();

		sptr = mod->GetModuleFileName(sbuff);
		this->modNames.Add(Text::String::NewP(sbuff, sptr));
		mod->GetModuleAddress(baseAddr, size);
		this->modBaseAddrs.Add(baseAddr);
		this->modSizes.Add(size);

		mod.Delete();
	}
}

Manage::SymbolResolver::~SymbolResolver()
{
	this->modNames.FreeAll();
	SymCleanup((HANDLE)this->proc->GetHandle());
}

UTF8Char *Manage::SymbolResolver::ResolveName(UTF8Char *buff, UInt64 address)
{
	UOSInt i;
	UOSInt j;
	SYMBOL_INFO *symb;
	UInt8 tmpBuff[sizeof(SYMBOL_INFO) + 256];
	DWORD64 disp;
	Bool found = false;
	NotNullPtr<Text::String> name;

	i = this->modNames.GetCount();
	while (i-- > 0)
	{
		if (((UInt64)address) >= (UInt64)this->modBaseAddrs.GetItem(i) && ((UInt64)address) < (UInt64)(this->modBaseAddrs.GetItem(i) + this->modSizes.GetItem(i)))
		{
			name = Text::String::OrEmpty(this->modNames.GetItem(i));
			j = name->LastIndexOf('\\');
			buff = Text::StrConcatC(buff, &name->v[j + 1], name->leng - j - 1);
			found = true;
			break;
		}
	}

	symb = (SYMBOL_INFO*)tmpBuff;
	symb->SizeOfStruct = sizeof(SYMBOL_INFO);
	symb->MaxNameLen = 256;

	BOOL ret;
	if ((ret = SymFromAddr(this->proc->GetHandle(), address, &disp, symb)) == 0)
	{
		UInt8 jmpBuff[5];
		OSInt readSize;
		if (ReadProcessMemory((HANDLE)this->proc->GetHandle(), (void*)address, jmpBuff, 5, (SIZE_T*)&readSize) && readSize == 5)
		{
			if (jmpBuff[0] == 0xe9)
			{
				address = address + 5 + (UInt64)(Int64)*(Int32*)&jmpBuff[1];
				ret = SymFromAddr(this->proc->GetHandle(), address, &disp, symb);
			}
		}
	}
	if (ret)
	{
		if (found)
		{
			buff = Text::StrConcatC(buff, UTF8STRC("!"));
		}
		buff = Text::StrConcat(buff, (const UTF8Char*)symb->Name);
		found = true;
		if (address != symb->Address)
		{
			buff = Text::StrConcatC(buff, UTF8STRC("+0x"));
			buff = Text::StrHexVal32(buff, (UInt32)address - (UInt32)symb->Address);
		}
	}
	else if (found)
	{
		if (address != this->modBaseAddrs.GetItem(i))
		{
			buff = Text::StrConcatC(buff, UTF8STRC("+0x"));
			buff = Text::StrHexVal32(buff, (UInt32)address - (UInt32)this->modBaseAddrs.GetItem(i));
		}
	}

	IMAGEHLP_LINE64 line;
	UInt32 displacement;
	line.SizeOfStruct = sizeof(line);
	if (SymGetLineFromAddr64(this->proc->GetHandle(), address, (DWORD*)&displacement, &line))
	{
		UOSInt i = Text::StrLastIndexOfChar(line.FileName, '\\');
		buff = Text::StrConcatC(buff, UTF8STRC(" "));
		buff = Text::StrConcat(buff, (const UTF8Char*)&line.FileName[i + 1]);
		buff = Text::StrConcatC(buff, UTF8STRC("("));
		buff = Text::StrInt32(buff, (Int32)line.LineNumber);
		buff = Text::StrConcatC(buff, UTF8STRC(")"));
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
