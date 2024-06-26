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
	Optional<Text::String> s;
	i = this->chapterNames.GetCount();
	while (i-- > 0)
	{
		this->chapterNames.GetItemNoCheck(i)->Release();
		s = this->chapterArtists.GetItem(i);
		OPTSTR_DEL(s);
	}
}

void Media::ChapterInfo::AddChapter(UInt32 chapterTime, Text::CStringNN chapterName, Text::CString chapterArtist)
{
	UOSInt i = this->chapterTimes.SortedInsert(chapterTime);
	this->chapterNames.Insert(i, Text::String::New(chapterName));
	this->chapterArtists.Insert(i, Text::String::NewOrNull(chapterArtist));
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

Optional<Text::String> Media::ChapterInfo::GetChapterName(UOSInt index)
{
	return this->chapterNames.GetItem(index);
}

Optional<Text::String> Media::ChapterInfo::GetChapterArtist(UOSInt index)
{
	return this->chapterArtists.GetItem(index);
}
