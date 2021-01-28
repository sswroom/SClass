#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ChapterInfo.h"
#include "Text/MyString.h"

Media::ChapterInfo::ChapterInfo()
{
	NEW_CLASS(this->chapterNames, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->chapterArtists, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->chapterTimes, Data::ArrayListInt32());
}

Media::ChapterInfo::~ChapterInfo()
{
	OSInt i;
	const UTF8Char *csptr;
	i = this->chapterNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->chapterNames->GetItem(i));
		csptr = this->chapterArtists->GetItem(i);
		SDEL_TEXT(csptr);
	}
	DEL_CLASS(this->chapterNames);
	DEL_CLASS(this->chapterArtists);
	DEL_CLASS(this->chapterTimes);
}

void Media::ChapterInfo::AddChapter(Int32 chapterTime, const UTF8Char *chapterName, const UTF8Char *chapterArtist)
{
	OSInt i = this->chapterTimes->SortedInsert(chapterTime);
	this->chapterNames->Insert(i, Text::StrCopyNew(chapterName));
	if (chapterArtist)
	{
		this->chapterArtists->Insert(i, Text::StrCopyNew(chapterArtist));
	}
	else
	{
		this->chapterArtists->Insert(i, 0);
	}
}

OSInt Media::ChapterInfo::GetChapterIndex(Int32 currTime)
{
	OSInt i = this->chapterTimes->SortedIndexOf(currTime);
	if (i >= 0)
		return i;
	else
		return ~i - 1;
}

Int32 Media::ChapterInfo::GetChapterTime(OSInt index)
{
	return this->chapterTimes->GetItem(index);
}

OSInt Media::ChapterInfo::GetChapterCnt()
{
	return this->chapterTimes->GetCount();
}

const UTF8Char *Media::ChapterInfo::GetChapterName(OSInt index)
{
	return this->chapterNames->GetItem(index);
}

const UTF8Char *Media::ChapterInfo::GetChapterArtist(OSInt index)
{
	return this->chapterArtists->GetItem(index);
}
