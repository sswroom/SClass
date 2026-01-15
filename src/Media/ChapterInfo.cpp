#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ChapterInfo.h"
#include "Text/MyString.h"

Media::ChapterInfo::ChapterInfo()
{
}

Media::ChapterInfo::~ChapterInfo()
{
	UIntOS i;
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
	UIntOS i = this->chapterTimes.SortedInsert(chapterTime);
	this->chapterNames.Insert(i, Text::String::New(chapterName));
	this->chapterArtists.Insert(i, Text::String::NewOrNull(chapterArtist));
}

UIntOS Media::ChapterInfo::GetChapterIndex(Data::Duration currTime)
{
	IntOS i = this->chapterTimes.SortedIndexOf((UInt32)currTime.GetTotalMS());
	if (i >= 0)
		return (UIntOS)i;
	else
		return (UIntOS)(~i - 1);
}

UInt32 Media::ChapterInfo::GetChapterTime(UIntOS index)
{
	return this->chapterTimes.GetItem(index);
}

UIntOS Media::ChapterInfo::GetChapterCnt()
{
	return this->chapterTimes.GetCount();
}

Optional<Text::String> Media::ChapterInfo::GetChapterName(UIntOS index)
{
	return this->chapterNames.GetItem(index);
}

Optional<Text::String> Media::ChapterInfo::GetChapterArtist(UIntOS index)
{
	return this->chapterArtists.GetItem(index);
}
