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
		Text::String *name;
		const UTF8Char *val;

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
					this->names.Put(name, val);
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
	Data::ArrayList<const UTF8Char*> *nameList = this->names.GetValues();
	i = nameList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(nameList->GetItem(i));
	}
#ifndef _WIN32_WCE
	if (envs)
	{
		FreeEnvironmentStringsW((LPWCH)envs);

	}
#endif
}

const UTF8Char *Manage::EnvironmentVar::GetValue(Text::CString name)
{
	return this->names.Get(name);
}

void Manage::EnvironmentVar::SetValue(Text::CString name, Text::CString val)
{
	const WChar *wname = Text::StrToWCharNew(name.v);
	const WChar *wval = Text::StrToWCharNew(val.v);
	SetEnvironmentVariableW(wname, wval);
	Text::StrDelNew(wname);
	Text::StrDelNew(wval);
}

UTF8Char *Manage::EnvironmentVar::GetEnvValue(UTF8Char *buff, Text::CString name)
{
#ifndef _WIN32_WCE
	WChar wbuff[512];
	const WChar *wptr = Text::StrToWCharNew(name.v);
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
