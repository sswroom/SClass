#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/EnvironmentVar.h"
#include "Text/MyString.h"
#include <stdlib.h>
#include <unistd.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__sun)
extern char **environ;
#endif

Manage::EnvironmentVar::EnvironmentVar()
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	this->envs = 0;
	i = 0;
	while (environ[i])
	{
		j = Text::StrIndexOfCharCh(environ[i], '=');
		if (j != INVALID_INDEX && j > 0)
		{
			sptr = Text::StrConcatC(sbuff, (const UTF8Char*)environ[i], j);
			this->names.Put(CSTRP(sbuff, sptr), (const UTF8Char*)&environ[i][j + 1]);
		}
		i++;
	}
}

Manage::EnvironmentVar::~EnvironmentVar()
{

}

UnsafeArrayOpt<const UTF8Char> Manage::EnvironmentVar::GetValue(Text::CStringNN name)
{
	return this->names.Get(name);
}

void Manage::EnvironmentVar::SetValue(Text::CStringNN name, Text::CStringNN val)
{
	setenv((const Char*)name.v.Ptr(), (const Char*)val.v.Ptr(), 1);
}

UnsafeArrayOpt<UTF8Char> Manage::EnvironmentVar::GetEnvValue(UnsafeArray<UTF8Char> buff, Text::CString name)
{
	char *v = getenv((const Char*)name.v.Ptr());
	if (v)
	{
		return Text::StrConcat(buff, (const UTF8Char*)v);
	}
	return 0;
}
