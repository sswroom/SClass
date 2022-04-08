#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Sync/Event.h"
#include "Text/HKSCSFix.h"
#include "Text/MyString.h"

Text::HKSCSFix::HKSCSFix()
{
	UTF8Char fileName[512];
	UTF8Char dir[512];
	UTF8Char *sptr;
	this->tab = 0;
	IO::Path::GetProcessFileName(fileName);
	sptr = IO::Path::GetFileDirectory(dir, fileName);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("HKSCSTab.dat"));
	IO::FileStream file(CSTRP(dir, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!file.IsError())
	{
		if (file.GetLength() == 32768)
		{
			this->tab = MemAlloc(Int32, 8192);
			file.Read((UInt8*)this->tab, 32768);
		}
	}
}

Text::HKSCSFix::~HKSCSFix()
{
	if (this->tab)
	{
		MemFree(this->tab);
		this->tab = 0;
	}
}

void Text::HKSCSFix::FixString(UTF16Char *s)
{
	UTF16Char c;
	if (this->tab == 0)
		return;

	while ((c = *s++) != 0)
	{
		if (c >= 0xe000)
		{
			if ((c = (UTF16Char)this->tab[c - 0xe000]) != 0)
				s[-1] = c;
		}
	}
}

void Text::HKSCSFix::FixString(UTF32Char *s)
{
	UTF32Char c;
	if (this->tab == 0)
		return;

	while ((c = *s++) != 0)
	{
		if (c >= 0xe000 && c < 0x10000)
		{
			if ((c = (UTF32Char)this->tab[c - 0xe000]) != 0)
				s[-1] = c;
		}
	}
}

Bool Text::HKSCSFix::IsError()
{
	return tab == 0;
}
