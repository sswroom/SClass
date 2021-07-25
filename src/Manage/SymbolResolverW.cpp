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

Manage::SymbolResolver::SymbolResolver(Manage::Process *proc)
{
	UOSInt i;
	UOSInt j;
	UOSInt baseAddr;
	UOSInt size;
	UTF8Char sbuff[256];
	NEW_CLASS(this->modNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->modBaseAddrs, Data::ArrayListUInt64());
	NEW_CLASS(this->modSizes, Data::ArrayListUInt64());
	this->proc = proc;
	SymInitialize((HANDLE)this->proc->GetHandle(), 0, TRUE);

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
			buff = Text::StrConcat(buff, (const UTF8Char*)"!");
		}
		buff = Text::StrConcat(buff, (const UTF8Char*)symb->Name);
		found = true;
		if (address != symb->Address)
		{
			buff = Text::StrConcat(buff, (const UTF8Char*)"+0x");;
			buff = Text::StrHexVal32(buff, (UInt32)address - (UInt32)symb->Address);
		}
	}
	else if (found)
	{
		if (address != (UInt64)this->modBaseAddrs->GetItem(i))
		{
			buff = Text::StrConcat(buff, (const UTF8Char*)"+0x");;
			buff = Text::StrHexVal32(buff, (UInt32)address - (UInt32)this->modBaseAddrs->GetItem(i));
		}
	}

	IMAGEHLP_LINE64 line;
	UInt32 displacement;
	line.SizeOfStruct = sizeof(line);
	if (SymGetLineFromAddr64(this->proc->GetHandle(), address, (DWORD*)&displacement, &line))
	{
		UOSInt i = Text::StrLastIndexOf(line.FileName, '\\');
		buff = Text::StrConcat(buff, (const UTF8Char*)" ");
		buff = Text::StrConcat(buff, (const UTF8Char*)&line.FileName[i + 1]);
		buff = Text::StrConcat(buff, (const UTF8Char*)"(");
		buff = Text::StrInt32(buff, (Int32)line.LineNumber);
		buff = Text::StrConcat(buff, (const UTF8Char*)")");
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

UInt64 Manage::SymbolResolver::GetModuleSize(UOSInt index)
{
	return this->modSizes->GetItem(index);
}
