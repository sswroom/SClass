#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/EnvironmentVar.h"
#include "Text/MyStringW.h"
#include <windows.h>

Manage::EnvironmentVar::EnvironmentVar()
{
	WChar wbuff[256];
	WChar *dptr;
	WChar c;
	const WChar *currPtr;
	this->envs = 0;
#ifndef _WIN32_WCE
	const WChar *envs;
	this->envs = (void*)(envs = (const WChar *)GetEnvironmentStringsW());
	if (envs)
	{
		NN<Text::String> name;
		UnsafeArray<const UTF8Char> val;

		currPtr = envs;
		while (*currPtr)
		{
			dptr = wbuff;
			while ((c = *currPtr++) != 0)
			{
				if (c == '=')
				{
					*dptr = 0;
					name = Text::String::NewNotNull(wbuff);
					val = Text::StrToUTF8New(currPtr);
					this->names.PutNN(name, val);
					name->Release();
					while (*currPtr++);
					break;
				}
				else
				{
					*dptr++ = c;
				}
			}
		}
	}
#endif
}

Manage::EnvironmentVar::~EnvironmentVar()
{
	UOSInt i;
	NN<const Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> nameList = this->names.GetValues();
	UnsafeArray<const UTF8Char> name;
	i = nameList->GetCount();
	while (i-- > 0)
	{
		if (nameList->GetItem(i).SetTo(name))
			Text::StrDelNew(name);
	}
#ifndef _WIN32_WCE
	if (envs)
	{
		FreeEnvironmentStringsW((LPWCH)envs);

	}
#endif
}

UnsafeArrayOpt<const UTF8Char> Manage::EnvironmentVar::GetValue(Text::CStringNN name)
{
	return this->names.Get(name);
}

void Manage::EnvironmentVar::SetValue(Text::CStringNN name, Text::CStringNN val)
{
	const WChar *wname = Text::StrToWCharNew(name.v);
	const WChar *wval = Text::StrToWCharNew(val.v);
	SetEnvironmentVariableW(wname, wval);
	Text::StrDelNew(wname);
	Text::StrDelNew(wval);
}

UnsafeArrayOpt<UTF8Char> Manage::EnvironmentVar::GetEnvValue(UnsafeArray<UTF8Char> buff, Text::CStringNN name)
{
#ifndef _WIN32_WCE
	WChar wbuff[512];
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(name.v);
	UInt32 retSize = GetEnvironmentVariableW(wptr, wbuff, 512);
	Text::StrDelNew(wptr);
	if (retSize == 0)
		return 0;
	else if (retSize > 512)
		return 0;
	else
		return Text::StrWChar_UTF8C(buff, wbuff, retSize);
#else
	return 0;
#endif
}
