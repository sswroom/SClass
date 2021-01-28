#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CodeScanner.h"
#include "Text/MyString.h"

IO::CodeScanner::CodeScanner(const UTF8Char *devName)
{
	this->devName = Text::StrCopyNew(devName);
}

IO::CodeScanner::~CodeScanner()
{
	Text::StrDelNew(this->devName);
}

const UTF8Char *IO::CodeScanner::GetDevName()
{
	return this->devName;
}
