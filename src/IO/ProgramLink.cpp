#include "Stdafx.h"
#include "IO/ProgramLink.h"

IO::ProgramLink::ProgramLink()
{
	this->type = 0;
	this->name = 0;
	this->genericName = 0;
	this->version = 0;
	this->comment = 0;
	this->mimeTypes = 0;
	this->categories = 0;
	this->keywords = 0;
	this->cmdLine = 0;
	this->noDisplay = false;
	this->startupNotify = false;
	this->terminal = false;
	this->icon = 0;
}

IO::ProgramLink::~ProgramLink()
{
	SDEL_STRING(this->type);
	SDEL_STRING(this->name);
	SDEL_STRING(this->genericName);
	SDEL_STRING(this->version);
	SDEL_STRING(this->comment);
	SDEL_STRING(this->categories);
	SDEL_STRING(this->keywords);
	SDEL_STRING(this->mimeTypes);
	SDEL_STRING(this->cmdLine);
	SDEL_STRING(this->icon);
}

Text::String *IO::ProgramLink::GetType() const
{
	return this->type;
}

void IO::ProgramLink::SetType(Text::String *type)
{
	SDEL_STRING(this->type);
	this->type = SCOPY_STRING(type);
}

void IO::ProgramLink::SetType(Text::CString type)
{
	SDEL_STRING(this->type);
	this->type = Text::String::NewOrNull(type);
}

Text::String *IO::ProgramLink::GetName() const
{
	return this->name;
}

void IO::ProgramLink::SetName(Text::String *name)
{
	SDEL_STRING(this->name);
	this->name = SCOPY_STRING(name);
}

void IO::ProgramLink::SetName(Text::CString name)
{
	SDEL_STRING(this->name);
	this->name = Text::String::NewOrNull(name);
}

Text::String *IO::ProgramLink::GetGenericName() const
{
	return this->genericName;
}

void IO::ProgramLink::SetGenericName(Text::String *genericName)
{
	SDEL_STRING(this->genericName);
	this->genericName = SCOPY_STRING(genericName);
}

void IO::ProgramLink::SetGenericName(Text::CString genericName)
{
	SDEL_STRING(this->genericName);
	this->genericName = Text::String::NewOrNull(genericName);
}

Text::String *IO::ProgramLink::GetVersion() const
{
	return this->version;
}

void IO::ProgramLink::SetVersion(Text::String *version)
{
	SDEL_STRING(this->version);
	this->version = SCOPY_STRING(version);
}

void IO::ProgramLink::SetVersion(Text::CString version)
{
	SDEL_STRING(this->version);
	this->version = Text::String::NewOrNull(version);
}

Text::String *IO::ProgramLink::GetComment() const
{
	return this->comment;
}

void IO::ProgramLink::SetComment(Text::String *comment)
{
	SDEL_STRING(this->comment);
	this->comment = SCOPY_STRING(comment);
}

void IO::ProgramLink::SetComment(Text::CString comment)
{
	SDEL_STRING(this->comment);
	this->comment = Text::String::NewOrNull(comment);
}

Text::String *IO::ProgramLink::GetMimeTypes() const
{
	return this->mimeTypes;
}

void IO::ProgramLink::SetMimeTypes(Text::String *mimeTypes)
{
	SDEL_STRING(this->mimeTypes);
	this->mimeTypes = SCOPY_STRING(mimeTypes);
}

void IO::ProgramLink::SetMimeTypes(Text::CString mimeTypes)
{
	SDEL_STRING(this->mimeTypes);
	this->mimeTypes = Text::String::NewOrNull(mimeTypes);
}

Text::String *IO::ProgramLink::GetCategories() const
{
	return this->categories;
}

void IO::ProgramLink::SetCategories(Text::String *categories)
{
	SDEL_STRING(this->categories);
	this->categories = SCOPY_STRING(categories);
}

void IO::ProgramLink::SetCategories(Text::CString categories)
{
	SDEL_STRING(this->categories);
	this->categories = Text::String::NewOrNull(categories);
}

Text::String *IO::ProgramLink::GetKeywords() const
{
	return this->keywords;
}

void IO::ProgramLink::SetKeywords(Text::String *keywords)
{
	SDEL_STRING(this->keywords);
	this->keywords = SCOPY_STRING(keywords);
}

void IO::ProgramLink::SetKeywords(Text::CString keywords)
{
	SDEL_STRING(this->keywords);
	this->keywords = Text::String::NewOrNull(keywords);
}

Text::String *IO::ProgramLink::GetCmdLine() const
{
	return this->cmdLine;
}

void IO::ProgramLink::SetCmdLine(Text::String *cmdLine)
{
	SDEL_STRING(this->cmdLine);
	this->cmdLine = SCOPY_STRING(cmdLine);
}

void IO::ProgramLink::SetCmdLine(Text::CString cmdLine)
{
	SDEL_STRING(this->cmdLine);
	this->cmdLine = Text::String::NewOrNull(cmdLine);
}

Bool IO::ProgramLink::IsNoDisplay() const
{
	return this->noDisplay;
}

void IO::ProgramLink::SetNoDisplay(Bool noDisplay)
{
	this->noDisplay = noDisplay;
}

Bool IO::ProgramLink::IsStartupNotify() const
{
	return this->startupNotify;
}

void IO::ProgramLink::SetStartupNotify(Bool startupNotify)
{
	this->startupNotify = startupNotify;
}

Bool IO::ProgramLink::HasTerminal() const
{
	return this->terminal;
}

void IO::ProgramLink::SetTerminal(Bool terminal)
{
	this->terminal = terminal;
}

Text::String *IO::ProgramLink::GetIcon() const
{
	return this->icon;
}

void IO::ProgramLink::SetIcon(Text::String *icon)
{
	SDEL_STRING(this->icon);
	this->icon = SCOPY_STRING(icon);
}

void IO::ProgramLink::SetIcon(Text::CString icon)
{
	SDEL_STRING(this->icon);
	this->icon = Text::String::NewOrNull(icon);
}
