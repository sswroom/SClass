#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganBook.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganBook::OrganBook()
{
	this->id = 0;
	this->title = nullptr;
	this->dispAuthor = nullptr;
	this->press = nullptr;
	this->groupId = 0;
	this->url = nullptr;
}

SSWR::OrganMgr::OrganBook::~OrganBook()
{
	OPTSTR_DEL(this->title);
	OPTSTR_DEL(this->dispAuthor);
	OPTSTR_DEL(this->press);
	OPTSTR_DEL(this->url);
}

Int32 SSWR::OrganMgr::OrganBook::GetBookId()
{
	return this->id;
}

void SSWR::OrganMgr::OrganBook::SetBookId(Int32 bookId)
{
	this->id = bookId;
}

Optional<Text::String> SSWR::OrganMgr::OrganBook::GetTitle()
{
	return this->title;
}

void SSWR::OrganMgr::OrganBook::SetTitle(Text::CString title)
{
	OPTSTR_DEL(this->title);
	this->title = Text::String::NewOrNull(title);
}

void SSWR::OrganMgr::OrganBook::SetTitle(Text::StringBase<UTF8Char> *title)
{
	OPTSTR_DEL(this->title);
	this->title = Text::String::New(title->v, title->leng);
}

Optional<Text::String> SSWR::OrganMgr::OrganBook::GetDispAuthor()
{
	return this->dispAuthor;
}

void SSWR::OrganMgr::OrganBook::SetDispAuthor(Text::CString dispAuthor)
{
	OPTSTR_DEL(this->dispAuthor);
	this->dispAuthor = Text::String::NewOrNull(dispAuthor);
}

void SSWR::OrganMgr::OrganBook::SetDispAuthor(Text::StringBase<UTF8Char> *dispAuthor)
{
	OPTSTR_DEL(this->dispAuthor);
	this->dispAuthor = Text::String::New(dispAuthor->v, dispAuthor->leng).Ptr();
}

Optional<Text::String> SSWR::OrganMgr::OrganBook::GetPress()
{
	return this->press;
}

void SSWR::OrganMgr::OrganBook::SetPress(Text::CString press)
{
	OPTSTR_DEL(this->press);
	this->press = Text::String::NewOrNull(press);
}

void SSWR::OrganMgr::OrganBook::SetPress(Text::StringBase<UTF8Char> *press)
{
	OPTSTR_DEL(this->press);
	this->press = Text::String::New(press->v, press->leng).Ptr();
}

Data::Timestamp SSWR::OrganMgr::OrganBook::GetPublishDate()
{
	return this->publishDate;
}

void SSWR::OrganMgr::OrganBook::SetPublishDate(const Data::Timestamp &publishDate)
{
	this->publishDate = publishDate;
}

Int32 SSWR::OrganMgr::OrganBook::GetGroupId()
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganBook::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Optional<Text::String> SSWR::OrganMgr::OrganBook::GetURL()
{
	return this->url;
}

void SSWR::OrganMgr::OrganBook::SetURL(Text::CString url)
{
	OPTSTR_DEL(this->url);
	this->url = Text::String::NewOrNull(url);
}

void SSWR::OrganMgr::OrganBook::SetURL(Text::StringBase<UTF8Char> *url)
{
	OPTSTR_DEL(this->url);
	this->url = Text::String::New(url->v, url->leng);
}

void SSWR::OrganMgr::OrganBook::GetString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendI32(this->id);;
	sb->AppendC(UTF8STRC(": "));
	sb->AppendOpt(this->dispAuthor);
	sb->AppendC(UTF8STRC(" "));
	sb->AppendI32(Data::DateTimeUtil::DispYearI32(this->publishDate.GetTimeValue().year));
	sb->AppendC(UTF8STRC(". "));
	sb->AppendOpt(this->title);
	sb->AppendC(UTF8STRC(". "));
	sb->AppendOpt(this->press);
	sb->AppendC(UTF8STRC("."));
}
