#include "Stdafx.h"
#include "IO/RobustelStatus.h"

extern "C" {
#include "librouter.h"
}

Optional<Text::String> IO::RobustelStatus::GetStatus(UnsafeArray<const Char> name)
{
	MemLock();
	char *val = usi_get(name.Ptr());
	MemUnlock();
	Optional<Text::String> s = Text::String::NewOrNull((const UTF8Char*)val);
	if (val)
	{
		MemLock();
		free(val);
		MemUnlock();
	}
	return s;
}
