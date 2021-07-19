#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/EnvironmentVar.h"
#include "Text/MyStringW.h"
#include <windows.h>

Manage::EnvironmentVar::EnvironmentVar()
{
	WChar sbuff[256];
	WChar *dptr;
	WChar c;
	const WChar *currPtr;
	this->envs = 0;
	this->names = 0;
#ifndef _WIN32_WCE
	const WChar *envs;
	this->envs = (void*)(envs = (const WChar *)GetEnvironmentStringsW());
	if (envs)
	{
		const UTF8Char *name;
		const UTF8Char *val;
		NEW_CLASS(this->names, Data::ICaseStringUTF8Map<const UTF8Char *>());

		currPtr = envs;
		while (*currPtr)
		{
			dptr = sbuff;
			while ((c = *currPtr++) != 0)
			{
				if (c == '=')
				{
					*dptr = 0;
					name = Text::StrToUTF8New(sbuff);
					val = Text::StrToUTF8New(currPtr);
					this->names->Put(name, val);
					Text::StrDelNew(name);
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
	if (this->names)
	{
		UOSInt i;
		Data::ArrayList<const UTF8Char*> *nameList = this->names->GetValues();
		i = nameList->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(nameList->GetItem(i));
		}
		DEL_CLASS(this->names);
	}
#ifndef _WIN32_WCE
	if (envs)
	{
		FreeEnvironmentStringsW((LPWCH)envs);

	}
#endif
}

const UTF8Char *Manage::EnvironmentVar::GetValue(const UTF8Char *name)
{
	if (names == 0)
		return 0;
	return names->Get(name);
}

void Manage::EnvironmentVar::SetValue(const UTF8Char *name, const UTF8Char *val)
{
	const WChar *wname = Text::StrToWCharNew(name);
	const WChar *wval = Text::StrToWCharNew(val);
	SetEnvironmentVariableW(wname, wval);
	Text::StrDelNew(wname);
	Text::StrDelNew(wval);
}

UTF8Char *Manage::EnvironmentVar::GetEnvValue(UTF8Char *buff, const UTF8Char *name)
{
#ifndef _WIN32_WCE
	WChar sbuff[512];
	const WChar *wptr = Text::StrToWCharNew(name);
	UInt32 retSize = GetEnvironmentVariableW(wptr, sbuff, 512);
	Text::StrDelNew(wptr);
	if (retSize == 0)
		return 0;
	else if (retSize > 512)
		return 0;
	else
		return Text::StrWChar_UTF8C(buff, sbuff, retSize);
#else
	return 0;
#endif
}
