#include "Stdafx.h"
#include "IO/RobustelStatus.h"

extern "C" {
#include "librouter.h"
}

Text::String *IO::RobustelStatus::GetStatus(const Char *name)
{
	char *val = usi_get(name);
	Text::String *s = Text::String::NewOrNull((const UTF8Char*)val);
	if (val) free(val);
	return s;
}
