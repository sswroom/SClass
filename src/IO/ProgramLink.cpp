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
	OPTSTR_DEL(this->type);
	OPTSTR_DEL(this->name);
	OPTSTR_DEL(this->genericName);
	OPTSTR_DEL(this->version);
	OPTSTR_DEL(this->comment);
	OPTSTR_DEL(this->categories);
	OPTSTR_DEL(this->keywords);
	OPTSTR_DEL(this->mimeTypes);
	OPTSTR_DEL(this->cmdLine);
	OPTSTR_DEL(this->icon);
}

Optional<Text::String> IO::ProgramLink::GetType() const
{
	return this->type;
}

void IO::ProgramLink::SetType(Optional<Text::String> type)
{
	OPTSTR_DEL(this->type);
	this->type = Text::String::CopyOrNull(type);
}

void IO::ProgramLink::SetType(Text::CString type)
{
	OPTSTR_DEL(this->type);
	this->type = Text::String::NewOrNull(type);
}

Optional<Text::String> IO::ProgramLink::GetName() const
{
	return this->name;
}

void IO::ProgramLink::SetName(Optional<Text::String> name)
{
	OPTSTR_DEL(this->name);
	this->name = Text::String::CopyOrNull(name);
}

void IO::ProgramLink::SetName(Text::CString name)
{
	OPTSTR_DEL(this->name);
	this->name = Text::String::NewOrNull(name);
}

Optional<Text::String> IO::ProgramLink::GetGenericName() const
{
	return this->genericName;
}

void IO::ProgramLink::SetGenericName(Optional<Text::String> genericName)
{
	OPTSTR_DEL(this->genericName);
	this->genericName = Text::String::CopyOrNull(genericName);
}

void IO::ProgramLink::SetGenericName(Text::CString genericName)
{
	OPTSTR_DEL(this->genericName);
	this->genericName = Text::String::NewOrNull(genericName);
}

Optional<Text::String> IO::ProgramLink::GetVersion() const
{
	return this->version;
}

void IO::ProgramLink::SetVersion(Optional<Text::String> version)
{
	OPTSTR_DEL(this->version);
	this->version = Text::String::CopyOrNull(version);
}

void IO::ProgramLink::SetVersion(Text::CString version)
{
	OPTSTR_DEL(this->version);
	this->version = Text::String::NewOrNull(version);
}

Optional<Text::String> IO::ProgramLink::GetComment() const
{
	return this->comment;
}

void IO::ProgramLink::SetComment(Optional<Text::String> comment)
{
	OPTSTR_DEL(this->comment);
	this->comment = Text::String::CopyOrNull(comment);
}

void IO::ProgramLink::SetComment(Text::CString comment)
{
	OPTSTR_DEL(this->comment);
	this->comment = Text::String::NewOrNull(comment);
}

Optional<Text::String> IO::ProgramLink::GetMimeTypes() const
{
	return this->mimeTypes;
}

void IO::ProgramLink::SetMimeTypes(Optional<Text::String> mimeTypes)
{
	OPTSTR_DEL(this->mimeTypes);
	this->mimeTypes = Text::String::CopyOrNull(mimeTypes);
}

void IO::ProgramLink::SetMimeTypes(Text::CString mimeTypes)
{
	OPTSTR_DEL(this->mimeTypes);
	this->mimeTypes = Text::String::NewOrNull(mimeTypes);
}

Optional<Text::String> IO::ProgramLink::GetCategories() const
{
	return this->categories;
}

void IO::ProgramLink::SetCategories(Optional<Text::String> categories)
{
	OPTSTR_DEL(this->categories);
	this->categories = Text::String::CopyOrNull(categories);
}

void IO::ProgramLink::SetCategories(Text::CString categories)
{
	OPTSTR_DEL(this->categories);
	this->categories = Text::String::NewOrNull(categories);
}

Optional<Text::String> IO::ProgramLink::GetKeywords() const
{
	return this->keywords;
}

void IO::ProgramLink::SetKeywords(Optional<Text::String> keywords)
{
	OPTSTR_DEL(this->keywords);
	this->keywords = Text::String::CopyOrNull(keywords);
}

void IO::ProgramLink::SetKeywords(Text::CString keywords)
{
	OPTSTR_DEL(this->keywords);
	this->keywords = Text::String::NewOrNull(keywords);
}

Optional<Text::String> IO::ProgramLink::GetCmdLine() const
{
	return this->cmdLine;
}

void IO::ProgramLink::SetCmdLine(Optional<Text::String> cmdLine)
{
	OPTSTR_DEL(this->cmdLine);
	this->cmdLine = Text::String::CopyOrNull(cmdLine);
}

void IO::ProgramLink::SetCmdLine(Text::CString cmdLine)
{
	OPTSTR_DEL(this->cmdLine);
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

Optional<Text::String> IO::ProgramLink::GetIcon() const
{
	return this->icon;
}

void IO::ProgramLink::SetIcon(Optional<Text::String> icon)
{
	OPTSTR_DEL(this->icon);
	this->icon = Text::String::CopyOrNull(icon);
}

void IO::ProgramLink::SetIcon(Text::CString icon)
{
	OPTSTR_DEL(this->icon);
	this->icon = Text::String::NewOrNull(icon);
}
