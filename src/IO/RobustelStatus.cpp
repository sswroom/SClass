#include "Stdafx.h"
#include "IO/RobustelStatus.h"

extern "C" {
#include "librouter.h"
}

Text::String *IO::RobustelStatus::GetStatus(const Char *name)
{
	MemLock();
	char *val = usi_get(name);
	MemUnlock();
	Text::String *s = Text::String::NewOrNull((const UTF8Char*)val);
	if (val)
	{
		MemLock();
		free(val);
		MemUnlock();
	}
	return s;
}
