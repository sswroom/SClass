#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CodeScanner.h"
#include "Text/MyString.h"

IO::CodeScanner::CodeScanner(Text::CString devName)
{
	this->devName = Text::String::New(devName);
}

IO::CodeScanner::~CodeScanner()
{
	this->devName->Release();
}

NN<Text::String> IO::CodeScanner::GetDevName() const
{
	return this->devName;
}
