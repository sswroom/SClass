#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/MonitorInfo.h"
#include "Text/MyString.h"

Media::MonitorInfo::MonitorInfo(Optional<MonitorHandle> hMonitor)
{
	this->isPrimary = true;
	this->left = 0;
	this->top = 0;
	this->right = 320;
	this->bottom = 240;
	this->name = Text::String::New(UTF8STRC("NullMonitor"));
	this->desc = 0;
	this->monId = 0;
}

Media::MonitorInfo::~MonitorInfo()
{
	this->name->Release();
	OPTSTR_DEL(this->desc);
	OPTSTR_DEL(this->monId);
}

NN<Text::String> Media::MonitorInfo::GetName() const
{
	return this->name;
}

Optional<Text::String> Media::MonitorInfo::GetDesc() const
{
	return this->desc;
}

Optional<Text::String> Media::MonitorInfo::GetMonitorID() const
{
	return this->monId;
}

Int32 Media::MonitorInfo::GetLeft() const
{
	return this->left;
}

Int32 Media::MonitorInfo::GetTop() const
{
	return this->top;
}

Int32 Media::MonitorInfo::GetPixelWidth() const
{
	return this->right - this->left;
}

Int32 Media::MonitorInfo::GetPixelHeight() const
{
	return this->bottom - this->top;
}

Bool Media::MonitorInfo::IsPrimary()
{
	return this->isPrimary;
}

