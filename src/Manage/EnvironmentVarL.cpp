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
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	i = 0;
	while (environ[i])
	{
		j = Text::StrIndexOfChar(environ[i], '=');
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

const UTF8Char *Manage::EnvironmentVar::GetValue(Text::CString name)
{
	return this->names.Get(name);
}

void Manage::EnvironmentVar::SetValue(Text::CString name, Text::CString val)
{
	setenv((const Char*)name.v, (const Char*)val.v, 1);
}

UTF8Char *Manage::EnvironmentVar::GetEnvValue(UTF8Char *buff, Text::CString name)
{
	char *v = getenv((const Char*)name.v);
	if (v)
	{
		return Text::StrConcat(buff, (const UTF8Char*)v);
	}
	return 0;
}
