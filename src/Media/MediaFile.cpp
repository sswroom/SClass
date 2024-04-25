#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "IO/StreamData.h"
#include "Media/IMediaSource.h"
#include "Media/MediaFile.h"

Media::MediaFile::MediaFile(NN<Text::String> name) : IO::ParsedObject(name)
{
	this->chapters = 0;
	this->releaseChapter = false;
}

Media::MediaFile::MediaFile(Text::CStringNN name) : IO::ParsedObject(name)
{
	this->chapters = 0;
	this->releaseChapter = false;
}

Media::MediaFile::~MediaFile()
{
	UOSInt i = this->sources.GetCount();
	Media::IMediaSource *src;
	while (i-- > 0)
	{
		if (this->keepSources.GetItem(i) == 0)
		{
			src = this->sources.GetItem(i);
			DEL_CLASS(src);
		}
	}
	if (this->chapters && this->releaseChapter)
	{
		DEL_CLASS(this->chapters);
	}
}

IO::ParserType Media::MediaFile::GetParserType() const
{
	return IO::ParserType::MediaFile;
}

UOSInt Media::MediaFile::AddSource(Media::IMediaSource *src, Int32 syncTime)
{
	UOSInt ret = this->sources.Add(src);
	this->syncTime.Add(syncTime);
	this->keepSources.Add(0);
	return ret;
}

Media::IMediaSource *Media::MediaFile::GetStream(UOSInt index, Int32 *syncTime)
{
	if (syncTime)
	{
		*syncTime = this->syncTime.GetItem(index);
	}
	return (Media::IMediaSource *)this->sources.GetItem(index);
}

void Media::MediaFile::KeepStream(UOSInt index, Bool toKeep)
{
	if (index >= this->sources.GetCount())
		return;

	this->keepSources.SetItem(index, toKeep?1:0);
}

Media::ChapterInfo *Media::MediaFile::GetChapterInfo() const
{
	return this->chapters;
}

void Media::MediaFile::SetChapterInfo(Media::ChapterInfo *chapters, Bool releaseChapter)
{
	if (this->chapters && this->releaseChapter)
	{
		DEL_CLASS(this->chapters);
	}
	this->chapters = chapters;
	this->releaseChapter = releaseChapter;
}

Bool Media::MediaFile::TrimFile(UInt32 trimTimeStart, Int32 trimTimeEnd)
{
	if (trimTimeStart == 0 && trimTimeEnd == -1)
		return true;
	Int32 syncTime;
	UOSInt i = this->sources.GetCount();
	Media::IMediaSource *src;
	while (i-- > 0)
	{
		src = this->sources.GetItem(i);
		syncTime = this->syncTime.GetItem(i);
		if (trimTimeEnd == -1)
		{
			src->TrimStream((UInt32)((Int32)trimTimeStart + syncTime), (UInt32)trimTimeEnd, &syncTime);
			this->syncTime.SetItem(i, syncTime);
		}
		else
		{
			src->TrimStream((UInt32)((Int32)trimTimeStart + syncTime), (UInt32)(trimTimeEnd + syncTime), &syncTime);
			this->syncTime.SetItem(i, syncTime);
		}
	}

	///////////////////////////////////
//	this->chapters = this->chapters;
	return true;
}
