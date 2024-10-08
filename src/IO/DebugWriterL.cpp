#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"
#include "Text/MyString.h"
#include <syslog.h>

struct IO::DebugWriter::ClassData
{
	UTF8Char buff[4096];
	UOSInt buffSize;
};

IO::DebugWriter::DebugWriter()
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->buffSize = 0;
	this->clsData->buff[0] = 0;
}

IO::DebugWriter::~DebugWriter()
{
	MemFree(this->clsData);
}

void IO::DebugWriter::Flush()
{
	UOSInt i;
	UnsafeArray<UTF8Char> sarr[2];
	i = Text::StrSplitLine(sarr, 2, this->clsData->buff);
	if (i == 2)
	{
		while (i == 2)
		{
			syslog(LOG_DEBUG, "%s", (const Char*)sarr[0].Ptr());
			i = Text::StrSplitLine(sarr, 2, sarr[1]);
		}
		i = Text::StrCharCnt(sarr[0]);
		MemCopyO(this->clsData->buff, sarr[0].Ptr(), i + 1);
		this->clsData->buffSize = i;
	}
}

Bool IO::DebugWriter::Write(Text::CStringNN str)
{
	str.ConcatTo(&this->clsData->buff[this->clsData->buffSize]);
	this->clsData->buffSize += str.leng;
	this->Flush();
	return true;
}

Bool IO::DebugWriter::WriteLine(Text::CStringNN str)
{
	str.ConcatTo(&this->clsData->buff[this->clsData->buffSize]);
	this->clsData->buffSize += str.leng;
	this->Flush();
	syslog(LOG_DEBUG, "%s", (const Char*)this->clsData->buff);
	this->clsData->buff[0] = 0;
	this->clsData->buffSize = 0;
	return true;
}

Bool IO::DebugWriter::WriteLine()
{
	syslog(LOG_DEBUG, "%s", (const Char*)this->clsData->buff);
	this->clsData->buff[0] = 0;
	this->clsData->buffSize = 0;
	return true;
}
