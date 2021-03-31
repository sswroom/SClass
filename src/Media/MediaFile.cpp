#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "IO/IStreamData.h"
#include "Media/IMediaSource.h"
#include "Media/MediaFile.h"

Media::MediaFile::MediaFile(const UTF8Char *name) : IO::ParsedObject(name)
{
	NEW_CLASS(this->sources, Data::ArrayList<Media::IMediaSource*>());
	NEW_CLASS(this->syncTime, Data::ArrayListInt32());
	NEW_CLASS(this->keepSources, Data::ArrayListInt32());
	this->chapters = 0;
	this->releaseChapter = false;
}

Media::MediaFile::~MediaFile()
{
	UOSInt i = this->sources->GetCount();
	Media::IMediaSource *src;
	while (i-- > 0)
	{
		if (this->keepSources->GetItem(i) == 0)
		{
			src = (Media::IMediaSource*)this->sources->GetItem(i);
			DEL_CLASS(src);
		}
	}
	DEL_CLASS(this->sources);
	DEL_CLASS(this->syncTime);
	DEL_CLASS(this->keepSources);
	if (this->chapters && this->releaseChapter)
	{
		DEL_CLASS(this->chapters);
	}
}

IO::ParsedObject::ParserType Media::MediaFile::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

UOSInt Media::MediaFile::AddSource(Media::IMediaSource *src, Int32 syncTime)
{
	this->sources->Add(src);
	this->syncTime->Add(syncTime);
	this->keepSources->Add(0);
	return this->sources->GetCount() - 1;
}

Media::IMediaSource *Media::MediaFile::GetStream(UOSInt index, Int32 *syncTime)
{
	if (syncTime)
	{
		*syncTime = this->syncTime->GetItem(index);
	}
	return (Media::IMediaSource *)this->sources->GetItem(index);
}

void Media::MediaFile::KeepStream(UOSInt index, Bool toKeep)
{
	if (index >= this->sources->GetCount())
		return;

	this->keepSources->SetItem(index, toKeep?1:0);
}

Media::ChapterInfo *Media::MediaFile::GetChapterInfo()
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

Bool Media::MediaFile::TrimFile(Int32 trimTimeStart, Int32 trimTimeEnd)
{
	if (trimTimeStart == 0 && trimTimeEnd == -1)
		return true;
	Int32 syncTime;
	UOSInt i = this->sources->GetCount();
	Media::IMediaSource *src;
	while (i-- > 0)
	{
		src = this->sources->GetItem(i);
		syncTime = this->syncTime->GetItem(i);
		if (trimTimeEnd == -1)
		{
			src->TrimStream(trimTimeStart + syncTime, trimTimeEnd, &syncTime);
			this->syncTime->SetItem(i, syncTime);
		}
		else
		{
			src->TrimStream(trimTimeStart + syncTime, trimTimeEnd + syncTime, &syncTime);
			this->syncTime->SetItem(i, syncTime);
		}
	}

	///////////////////////////////////
//	this->chapters = this->chapters;
	return true;
}
