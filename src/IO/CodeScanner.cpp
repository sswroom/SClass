#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CodeScanner.h"
#include "Text/MyString.h"

IO::CodeScanner::CodeScanner(const UTF8Char *devName)
{
	this->devName = Text::String::NewNotNull(devName);
}

IO::CodeScanner::~CodeScanner()
{
	this->devName->Release();
}

Text::String *IO::CodeScanner::GetDevName()
{
	return this->devName;
}
