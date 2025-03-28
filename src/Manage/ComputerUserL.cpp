#include "Stdafx.h"
#include "Manage/ComputerUser.h"
#include "Text/MyString.h"
#include <unistd.h>
#include <pwd.h>

UnsafeArrayOpt<UTF8Char> Manage::ComputerUser::GetProcessUser(UnsafeArray<UTF8Char> sbuff)
{
	uid_t uid = geteuid();
	passwd *pw = getpwuid(uid);
	if (pw)
	{
		return Text::StrConcat(sbuff, (const UTF8Char*)pw->pw_name);
	}
	return 0;
}
