#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganBook.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganBook::OrganBook()
{
	this->id = 0;
	this->title = 0;
	this->dispAuthor = 0;
	this->press = 0;
	NEW_CLASS(this->publishDate, Data::DateTime());
	this->groupId = 0;
	this->url = 0;
}

SSWR::OrganMgr::OrganBook::~OrganBook()
{
	SDEL_STRING(this->title);
	SDEL_STRING(this->dispAuthor);
	SDEL_STRING(this->press);
	DEL_CLASS(this->publishDate);
	SDEL_STRING(this->url);
}

Int32 SSWR::OrganMgr::OrganBook::GetBookId()
{
	return this->id;
}

void SSWR::OrganMgr::OrganBook::SetBookId(Int32 bookId)
{
	this->id = bookId;
}

Text::String *SSWR::OrganMgr::OrganBook::GetTitle()
{
	return this->title;
}

void SSWR::OrganMgr::OrganBook::SetTitle(const UTF8Char *title)
{
	SDEL_STRING(this->title);
	this->title = Text::String::NewNotNull(title);
}

void SSWR::OrganMgr::OrganBook::SetTitle(Text::StringBase<UTF8Char> *title)
{
	SDEL_STRING(this->title);
	this->title = Text::String::New(title->v, title->leng);
}

Text::String *SSWR::OrganMgr::OrganBook::GetDispAuthor()
{
	return this->dispAuthor;
}

void SSWR::OrganMgr::OrganBook::SetDispAuthor(const UTF8Char *dispAuthor)
{
	SDEL_STRING(this->dispAuthor);
	this->dispAuthor = Text::String::NewNotNull(dispAuthor);
}

void SSWR::OrganMgr::OrganBook::SetDispAuthor(Text::StringBase<UTF8Char> *dispAuthor)
{
	SDEL_STRING(this->dispAuthor);
	this->dispAuthor = Text::String::New(dispAuthor->v, dispAuthor->leng);
}

Text::String *SSWR::OrganMgr::OrganBook::GetPress()
{
	return this->press;
}

void SSWR::OrganMgr::OrganBook::SetPress(const UTF8Char *press)
{
	SDEL_STRING(this->press);
	this->press = Text::String::NewNotNull(press);
}

void SSWR::OrganMgr::OrganBook::SetPress(Text::StringBase<UTF8Char> *press)
{
	SDEL_STRING(this->press);
	this->press = Text::String::New(press->v, press->leng);
}

Data::DateTime *SSWR::OrganMgr::OrganBook::GetPublishDate()
{
	return this->publishDate;
}

void SSWR::OrganMgr::OrganBook::SetPublishDate(Data::DateTime *publishDate)
{
	this->publishDate->SetValue(publishDate);
}

Int32 SSWR::OrganMgr::OrganBook::GetGroupId()
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganBook::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Text::String *SSWR::OrganMgr::OrganBook::GetURL()
{
	return this->url;
}

void SSWR::OrganMgr::OrganBook::SetURL(const UTF8Char *url)
{
	SDEL_STRING(this->url);
	this->url = Text::String::NewNotNull(url);
}

void SSWR::OrganMgr::OrganBook::SetURL(Text::StringBase<UTF8Char> *url)
{
	SDEL_STRING(this->url);
	this->url = Text::String::New(url->v, url->leng);
}

void SSWR::OrganMgr::OrganBook::GetString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendI32(this->id);;
	sb->AppendC(UTF8STRC(": "));
	sb->Append(this->dispAuthor);
	sb->AppendC(UTF8STRC(" "));
	sb->AppendU16(this->publishDate->GetYear());
	sb->AppendC(UTF8STRC(". "));
	sb->Append(this->title);
	sb->AppendC(UTF8STRC(". "));
	sb->Append(this->press);
	sb->AppendC(UTF8STRC("."));
}
