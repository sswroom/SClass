#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ChapterInfo.h"
#include "Text/MyString.h"

Media::ChapterInfo::ChapterInfo()
{
}

Media::ChapterInfo::~ChapterInfo()
{
	UOSInt i;
	Text::String *s;
	i = this->chapterNames.GetCount();
	while (i-- > 0)
	{
		this->chapterNames.GetItem(i)->Release();
		s = this->chapterArtists.GetItem(i);
		SDEL_STRING(s);
	}
}

void Media::ChapterInfo::AddChapter(UInt32 chapterTime, Text::CString chapterName, Text::CString chapterArtist)
{
	UOSInt i = this->chapterTimes.SortedInsert(chapterTime);
	this->chapterNames.Insert(i, Text::String::New(chapterName));
	if (chapterArtist.leng > 0)
	{
		this->chapterArtists.Insert(i, Text::String::New(chapterArtist).Ptr());
	}
	else
	{
		this->chapterArtists.Insert(i, 0);
	}
}

UOSInt Media::ChapterInfo::GetChapterIndex(Data::Duration currTime)
{
	OSInt i = this->chapterTimes.SortedIndexOf((UInt32)currTime.GetTotalMS());
	if (i >= 0)
		return (UOSInt)i;
	else
		return (UOSInt)(~i - 1);
}

UInt32 Media::ChapterInfo::GetChapterTime(UOSInt index)
{
	return this->chapterTimes.GetItem(index);
}

UOSInt Media::ChapterInfo::GetChapterCnt()
{
	return this->chapterTimes.GetCount();
}

Text::String *Media::ChapterInfo::GetChapterName(UOSInt index)
{
	return this->chapterNames.GetItem(index);
}

Text::String *Media::ChapterInfo::GetChapterArtist(UOSInt index)
{
	return this->chapterArtists.GetItem(index);
}
