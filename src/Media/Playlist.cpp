#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/Playlist.h"
#include "Text/MyString.h"

void __stdcall Media::Playlist::OnPBEnd(void *userObj)
{
	Media::Playlist *me = (Media::Playlist*)userObj;
	Data::RandomOS random;
	UOSInt i = (UInt32)random.NextInt15() % me->entries.GetCount();
	me->OpenItem(i);
}

void Media::Playlist::FreeEntry(PlaylistEntry* ent)
{
	ent->title->Release();
	SDEL_STRING(ent->artist);
	ent->fileName->Release();
	MemFree(ent);
}

Media::Playlist::Playlist(Text::CStringNN sourceName, Parser::ParserList *parsers) : IO::ParsedObject(sourceName)
{
	this->parsers = parsers;
	this->player = 0;
	this->currFile = 0;
	this->playing = false;
}

Media::Playlist::~Playlist()
{
	if (this->player)
	{
		this->player->LoadMedia(0);
		this->player->SetEndHandler(0, 0);
		this->player = 0;
	}
	SDEL_CLASS(this->currFile);
	UOSInt i;
	i = this->entries.GetCount();
	while (i-- > 0)
	{
		FreeEntry(this->entries.GetItem(i));
	}
}

IO::ParserType Media::Playlist::GetParserType() const
{
	return IO::ParserType::Playlist;
}

Bool Media::Playlist::AddFile(Text::CStringNN fileName)
{
	Media::MediaFile *file;
	{
		IO::StmData::FileData fd(fileName, false);
		file = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
	}
	if (file == 0)
	{
		return false;
	}


	Media::ChapterInfo *chap = file->GetChapterInfo();
	Text::String *artist;
	UOSInt i;
	UOSInt j;
	Data::Duration nextTime;
	PlaylistEntry *ent;
	if (chap)
	{
		i = 0;
		j = chap->GetChapterCnt();
		while (i < j)
		{
			if (i >= j - 1)
			{
				nextTime = Data::Duration::Infinity();
			}
			else
			{
				nextTime = (Int32)chap->GetChapterTime(i + 1);
			}

			ent = MemAlloc(PlaylistEntry, 1);
			ent->fileName = Text::String::New(fileName.v, fileName.leng);
			ent->title = chap->GetChapterName(i)->Clone();
			artist = chap->GetChapterArtist(i);
			if (artist)
			{
				ent->artist = artist->Clone().Ptr();
			}
			else
			{
				ent->artist = 0;
			}
			ent->timeStart = chap->GetChapterTime(i);
			ent->timeEnd = nextTime;
			this->entries.Add(ent);

			i++;
		}
	}
	else
	{
		ent = MemAlloc(PlaylistEntry, 1);
		ent->fileName = Text::String::New(fileName.v, fileName.leng);
		i = Text::StrLastIndexOfCharC(fileName.v, fileName.leng, IO::Path::PATH_SEPERATOR);
		ent->title = Text::String::New(&fileName.v[i + 1], fileName.leng - i - 1);
		ent->artist = 0;
		ent->timeStart = 0;
		ent->timeEnd = -1;
		this->entries.Add(ent);
	}
	DEL_CLASS(file);
	return true;
}

Bool Media::Playlist::RemoveEntry(UOSInt index)
{
	PlaylistEntry *ent = this->entries.RemoveAt(index);
	if (ent == 0)
		return false;
	FreeEntry(ent);
	return true;
}

Bool Media::Playlist::AppendPlaylist(Media::Playlist *playlist)
{
	UOSInt i;
	UOSInt j;
	PlaylistEntry *ent;
	PlaylistEntry *plent;
	i = 0;
	j = playlist->entries.GetCount();
	while (i < j)
	{
		plent = playlist->entries.GetItem(i);
		ent = MemAlloc(PlaylistEntry, 1);
		ent->fileName = plent->fileName->Clone();
		ent->title = plent->title->Clone();
		if (plent->artist)
		{
			ent->artist = plent->artist->Clone().Ptr();
		}
		else
		{
			ent->artist = 0;
		}
		ent->timeStart = plent->timeStart;
		ent->timeEnd = plent->timeEnd;
		this->entries.Add(ent);
		i++;
	}
	return true;
}

void Media::Playlist::ClearFiles()
{
	UOSInt i = this->entries.GetCount();
	PlaylistEntry *ent;
	while (i-- > 0)
	{
		ent = this->entries.RemoveAt(i);
		FreeEntry(ent);
	}
	this->entries.Clear();
}

UOSInt Media::Playlist::GetCount() const
{
	return this->entries.GetCount();
}

Text::String *Media::Playlist::GetTitle(UOSInt index) const
{
	PlaylistEntry *ent = this->entries.GetItem(index);
	if (ent == 0)
		return 0;
	return ent->title.Ptr();
}

Text::String *Media::Playlist::GetArtist(UOSInt index) const
{
	PlaylistEntry *ent = this->entries.GetItem(index);
	if (ent == 0)
		return 0;
	return ent->artist;
}

Text::String *Media::Playlist::GetFileName(UOSInt index) const
{
	PlaylistEntry *ent = this->entries.GetItem(index);
	if (ent == 0)
		return 0;
	return ent->fileName.Ptr();
}

Data::Duration Media::Playlist::GetTimeStart(UOSInt index) const
{
	PlaylistEntry *ent = this->entries.GetItem(index);
	if (ent == 0)
		return 0;
	return ent->timeStart;
}

Data::Duration Media::Playlist::GetTimeEnd(UOSInt index) const
{
	PlaylistEntry *ent = this->entries.GetItem(index);
	if (ent == 0)
		return 0;
	return ent->timeEnd;
}

void Media::Playlist::SetPlayer(Media::IMediaPlayer *player)
{
	if (this->player)
	{
		this->player->SetEndHandler(0, 0);
		this->player = 0;
	}
	this->player = player;
	if (this->player)
	{
		this->player->SetEndHandler(OnPBEnd, this);
	}
}

Bool Media::Playlist::OpenItem(UOSInt index)
{
	PlaylistEntry *ent = this->entries.GetItem(index);
	if (ent == 0 || this->player == 0)
		return false;

	this->player->LoadMedia(0);
	SDEL_CLASS(this->currFile);
	{
		IO::StmData::FileData fd(ent->fileName, false);
		this->currFile = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
	}

	if (this->currFile == 0)
		return false;
	
	this->currFile->TrimFile((UInt32)ent->timeStart.GetTotalMS(), (Int32)ent->timeEnd.GetTotalMS());

	if (!this->player->LoadMedia(this->currFile))
		return false;
	if (this->playing)
	{
		return this->player->StartPlayback();
	}
	else
	{
		return true;
	}
}

Bool Media::Playlist::IsPlaying()
{
	return this->playing;
}

Bool Media::Playlist::StartPlayback()
{
	if (this->currFile == 0)
	{
		OnPBEnd(this);
	}

	if (this->player)
	{
		this->playing = this->player->StartPlayback();
		return this->playing;
	}
	return false;
}

Bool Media::Playlist::StopPlayback()
{
	this->playing = false;
	if (this->player)
	{
		this->player->StopPlayback();
		return true;
	}
	return false;
}

Bool Media::Playlist::PrevChapter()
{
	if (this->player == 0)
	{
		return false;
	}
	OnPBEnd(this);
	return true;
}

Bool Media::Playlist::NextChapter()
{
	if (this->player == 0)
	{
		return false;
	}
	OnPBEnd(this);
	return true;
}

Data::Duration Media::Playlist::GetCurrTime()
{
	if (this->player == 0)
	{
		return 0;
	}
	return this->player->GetCurrTime();
}
