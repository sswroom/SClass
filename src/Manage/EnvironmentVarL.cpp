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
	UOSInt i;
	UOSInt j;
	NEW_CLASS(this->names, Data::ICaseStringUTF8Map<const UTF8Char *>());
	i = 0;
	while (environ[i])
	{
		j = Text::StrIndexOf(environ[i], '=');
		if (j != INVALID_INDEX && j > 0)
		{
			Text::StrConcatC(sbuff, (const UTF8Char*)environ[i], j);
			this->names->Put(sbuff, (const UTF8Char*)&environ[i][j + 1]);
		}
		i++;
	}
}

Manage::EnvironmentVar::~EnvironmentVar()
{
	if (names)
	{
		DEL_CLASS(names);
	}
}

const UTF8Char *Manage::EnvironmentVar::GetValue(const UTF8Char *name)
{
	if (names == 0)
		return 0;
	return names->Get(name);
}

void Manage::EnvironmentVar::SetValue(const UTF8Char *name, const UTF8Char *val)
{
	setenv((const Char*)name, (const Char*)val, 1);
}

UTF8Char *Manage::EnvironmentVar::GetEnvValue(UTF8Char *buff, const UTF8Char *name)
{
	char *v = getenv((const Char*)name);
	if (v)
	{
		return Text::StrConcat(buff, (const UTF8Char*)v);
	}
	return 0;
}
