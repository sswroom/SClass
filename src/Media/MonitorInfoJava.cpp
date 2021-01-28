#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/MonitorInfo.h"
#include "Text/MyString.h"
\
Media::MonitorInfo::MonitorInfo(void *hMonitor)
{
	this->isPrimary = true;
	this->left = 0;
	this->top = 0;
	this->right = 320;
	this->bottom = 240;
	this->name = Text::StrCopyNew((const UTF8Char*)"Monitor");
	this->desc = 0;
	this->monId = 0;
}

Media::MonitorInfo::~MonitorInfo()
{
	SDEL_TEXT(this->name);
	SDEL_TEXT(this->desc);
	SDEL_TEXT(this->monId);
}

const UTF8Char *Media::MonitorInfo::GetName()
{
	return this->name;
}

const UTF8Char *Media::MonitorInfo::GetDesc()
{
	return this->desc;
}

const UTF8Char *Media::MonitorInfo::GetMonitorID()
{
	return this->monId;
}

Int32 Media::MonitorInfo::GetLeft()
{
	return this->left;
}

Int32 Media::MonitorInfo::GetTop()
{
	return this->top;
}

Int32 Media::MonitorInfo::GetPixelWidth()
{
	return this->right - this->left;
}

Int32 Media::MonitorInfo::GetPixelHeight()
{
	return this->bottom - this->top;
}

Bool Media::MonitorInfo::IsPrimary()
{
	return this->isPrimary;
}

